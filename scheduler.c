// scheduler.c
// runs tasks from the queue on one thread using RR (3s first round, 7s
// after) with SRJF to pick the shortest remaining program. shell tasks
// have burst -1 and run to completion in one round, never preempted.

#include "myshell.h"
#include "scheduler.h"

#include <semaphore.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>


// task type and state

typedef enum { TASK_SHELL, TASK_PROGRAM } TaskType;
typedef enum { ST_CREATED, ST_RUNNING, ST_WAITING, ST_ENDED } TaskState;

typedef struct Task {
    int        client_id;       // shown in the [N]/(N) log lines
    int        client_socket;   // where output is sent back
    TaskType   type;
    char       command[MAX_INPUT];
    int        n_total;         // burst from the client (-1 for shell)
    int        n_remaining;     // burst still owed
    int        rounds_done;     // 0 = first round (3s), >=1 picks 7s quantum
    long       arrival_seq;     // fcfs tiebreaker
    TaskState  state;
    pid_t      pid;             // child pid once forked, -1 otherwise
    int        stdout_fd;       // read end of child's stdout pipe
    char      *output;          // buffered output, sent when task ends
    int        output_len;
    struct Task *next;
} Task;


// shared state, all of this is protected by queue_lock

static Task            *queue_head = NULL;
static pthread_mutex_t  queue_lock = PTHREAD_MUTEX_INITIALIZER;

// counts tasks ready to be picked, lab 9 producer/consumer pattern
static sem_t tasks_available;

// task currently on the cpu, NULL when idle
static Task *running_task  = NULL;

// last task scheduled, used for the no-same-twice rule
static Task *last_run_task = NULL;

// counter for arrival_seq values
static long arrival_counter = 0;

// pipe used by client threads to wake the scheduler from select() when
// a new task should preempt the running one
static int preempt_pipe[2];

// print_lock lives in server.c, share it so logs dont interleave
extern pthread_mutex_t print_lock;

// gantt summary, e.g. 0)-P6-(3)-P7-(6)-P6-(13)
static char gantt_buf[2048] = "0)-";
static int  sim_clock = 0;


// helpers

// returns TASK_PROGRAM with N as burst if cmd starts with "demo N" or
// "./demo N", otherwise TASK_SHELL with burst -1
static void classify_command(const char *cmd, TaskType *type, int *n){
    const char *p = cmd;
    while(*p == ' ' || *p == '\t') p++;

    int matched = 0;
    if(strncmp(p, "./demo", 6) == 0 && (p[6] == ' ' || p[6] == '\t')){
        p += 6;
        matched = 1;
    }
    else if(strncmp(p, "demo", 4) == 0 && (p[4] == ' ' || p[4] == '\t')){
        p += 4;
        matched = 1;
    }

    if(matched){
        while(*p == ' ' || *p == '\t') p++;
        if(*p >= '0' && *p <= '9'){
            *type = TASK_PROGRAM;
            *n = atoi(p);
            return;
        }
    }

    *type = TASK_SHELL;
    *n = -1;
}

static void log_state(int client_id, const char *st, int n){
    const char *color;
    if (strcmp(st, "created")  == 0) color = "\e[34m"; // blue
    else if(strcmp(st, "started")  == 0) color = "\e[32m"; // green
    else if(strcmp(st, "running")  == 0) color = "\e[32m"; // green
    else if(strcmp(st, "waiting")  == 0) color = "\e[33m"; // yellow
    else if(strcmp(st, "ended")    == 0) color = "\e[31m"; // red
    else color = "\e[0m";

    pthread_mutex_lock(&print_lock);
    printf("(%d)--- %s%s\e[0m (%d)\n", client_id, color, st, n);
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);
}

static void log_bytes_sent(int client_id, int bytes){
    pthread_mutex_lock(&print_lock);
    printf("[%d]<<< %d bytes sent\n", client_id, bytes);
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);
}

static void log_gantt(){
    pthread_mutex_lock(&print_lock);
    printf("%s\n", gantt_buf);
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);
}

// queue helpers, all assume queue_lock is held by the caller

static void enqueue_locked(Task *t){
    t->next = NULL;
    if(queue_head == NULL){
        queue_head = t;
        return;
    }
    Task *p = queue_head;
    while(p->next != NULL) p = p->next;
    p->next = t;
}

static void remove_locked(Task *t){
    Task **pp = &queue_head;
    while(*pp != NULL){
        if(*pp == t){
            *pp = t->next;
            t->next = NULL;
            return;
        }
        pp = &(*pp)->next;
    }
}

// pick the next task: shell beats program, then SRJF, skipping the
// last-run task unless its the only one left, fcfs tiebreak
static Task *pick_next_task(){
    if(queue_head == NULL) return NULL;

    // any shell beats any program
    Task *shell = NULL;
    for(Task *p = queue_head; p != NULL; p = p->next){
        if(p->type == TASK_SHELL){
            if(shell == NULL || p->arrival_seq < shell->arrival_seq) shell = p;
        }
    }
    if(shell != NULL) return shell;

    // count programs so we know if no-same-twice has any alternative
    int total = 0;
    Task *only_one = NULL;
    for(Task *p = queue_head; p != NULL; p = p->next){
        if(p->type == TASK_PROGRAM){
            total++;
            only_one = p;
        }
    }
    if(total == 0) return NULL;
    if(total == 1) return only_one;

    // SRJF skipping last_run_task
    Task *best = NULL;
    for(Task *p = queue_head; p != NULL; p = p->next){
        if(p->type != TASK_PROGRAM) continue;
        if(p == last_run_task) continue;
        if(best == NULL ||
           p->n_remaining < best->n_remaining ||
           (p->n_remaining == best->n_remaining && p->arrival_seq < best->arrival_seq)){
            best = p;
        }
    }
    return best;
}

static void free_task(Task *t){
    if(t->stdout_fd >= 0) close(t->stdout_fd);
    if(t->output != NULL) free(t->output);
    free(t);
}


// shell task: fork+exec+wait, capture output, send back in one go
// (same protocol as phase 3, never preempted)

static char *run_shell_capture(const char *cmd_str, int *out_len, int *status){
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe");
        return NULL;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return NULL;
    }

    if(pid == 0){
        // child: redirect stdout/stderr to the pipe
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);

        // parse_input writes through its arg, copy first
        char buf[MAX_INPUT];
        strncpy(buf, cmd_str, MAX_INPUT - 1);
        buf[MAX_INPUT - 1] = '\0';

        Pipeline pl;
        init_pipeline(&pl);
        if(parse_input(buf, &pl) == -1) exit(EXIT_FAILURE);

        if(pl.num_commands == 1){
            apply_redirections(&pl.commands[0]);
            execvp(pl.commands[0].args[0], pl.commands[0].args);
            error_command_not_found(pl.commands[0].args[0]);
            exit(EXIT_FAILURE);
        }
        else{
            execute_pipes(&pl);
            exit(EXIT_SUCCESS);
        }
    }

    // parent reads everything, growing the buffer if needed
    close(fd[1]);

    int cap = BUFFER_SIZE;
    char *output = malloc(cap);
    if(output == NULL){
        perror("malloc");
        close(fd[0]);
        waitpid(pid, status, 0);
        return NULL;
    }

    int total = 0;
    int n;
    while((n = read(fd[0], output + total, cap - total - 1)) > 0){
        total += n;
        if(total >= cap - 1){
            cap *= 2;
            char *tmp = realloc(output, cap);
            if(tmp == NULL){
                free(output);
                close(fd[0]);
                waitpid(pid, status, 0);
                return NULL;
            }
            output = tmp;
        }
    }
    close(fd[0]);
    waitpid(pid, status, 0);
    output[total] = '\0';
    *out_len = total;
    return output;
}

static void run_shell_task(Task *task){
    int len = 0;
    int status = 0;
    char *out = run_shell_capture(task->command, &len, &status);

    if(out == NULL){
        const char *err = "Server error: failed to execute command\n";
        len = strlen(err);
        send(task->client_socket, &len, sizeof(int), 0);
        send(task->client_socket, err, len, 0);
        log_bytes_sent(task->client_id, len);
        log_state(task->client_id, "ended", -1);
        return;
    }

    send(task->client_socket, &len, sizeof(int), 0);
    if(len > 0) send(task->client_socket, out, len, 0);
    log_bytes_sent(task->client_id, len);
    log_state(task->client_id, "ended", -1);
    free(out);
}


// program task

// fork+exec the program once, pipe its stdout back to us. pid and
// stdout_fd are kept on the task so the next round just SIGCONTs
static int spawn_program(Task *task){
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return -1;
    }

    if(pid == 0){
        // child: stdout/stderr to pipe then exec
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);

        // simple split on whitespace, fine for ./demo N
        char buf[MAX_INPUT];
        strncpy(buf, task->command, MAX_INPUT - 1);
        buf[MAX_INPUT - 1] = '\0';

        char *argv[16];
        int argc = 0;
        char *tok = strtok(buf, " \t");
        while(tok != NULL && argc < 15){
            argv[argc++] = tok;
            tok = strtok(NULL, " \t");
        }
        argv[argc] = NULL;

        execvp(argv[0], argv);
        error_command_not_found(argv[0]);
        exit(EXIT_FAILURE);
    }

    close(fd[1]);
    task->pid = pid;
    task->stdout_fd = fd[0];
    // non-blocking so the drain at end of task doesnt stall on empty pipe
    int fl = fcntl(task->stdout_fd, F_GETFL, 0);
    fcntl(task->stdout_fd, F_SETFL, fl | O_NONBLOCK);
    return 0;
}

// append n bytes onto task->output, growing as needed
static void append_output(Task *task, const char *buf, int n){
    char *tmp = realloc(task->output, task->output_len + n + 1);
    if(tmp == NULL) return;
    task->output = tmp;
    memcpy(task->output + task->output_len, buf, n);
    task->output_len += n;
    task->output[task->output_len] = '\0';
}

// run task for one quantum or until it finishes or gets preempted
// returns 1 if task fully finished, 0 if it should go back on the queue
static int run_program_quantum(Task *task){
    int quantum = (task->rounds_done == 0) ? 3 : 7;
    time_t start = time(NULL);
    int finished = 0;
    int preempted = 0;

    while(1){
        int elapsed = (int)(time(NULL) - start);
        if(elapsed >= quantum) break;
        int remaining_q = quantum - elapsed;

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(task->stdout_fd, &rfds);
        FD_SET(preempt_pipe[0], &rfds);
        int maxfd = task->stdout_fd > preempt_pipe[0] ? task->stdout_fd : preempt_pipe[0];

        struct timeval tv;
        tv.tv_sec = remaining_q;
        tv.tv_usec = 0;

        int sel = select(maxfd + 1, &rfds, NULL, NULL, &tv);
        if(sel < 0){
            if(errno == EINTR) continue;
            perror("select");
            break;
        }
        if(sel == 0) break; // quantum is up

        if(FD_ISSET(task->stdout_fd, &rfds)){
            char rbuf[256];
            int r = read(task->stdout_fd, rbuf, sizeof(rbuf));
            if(r > 0){
                append_output(task, rbuf, r);
            }
            else if(r == 0){
                // pipe closed, child exited on its own
                finished = 1;
                break;
            }
        }

        if(FD_ISSET(preempt_pipe[0], &rfds)){
            // drain pending wakeup bytes
            char tmp[16];
            while(read(preempt_pipe[0], tmp, sizeof(tmp)) > 0) { }
            preempted = 1;
            break;
        }
    }

    int elapsed = (int)(time(NULL) - start);
    if(elapsed > task->n_remaining) elapsed = task->n_remaining;
    task->n_remaining -= elapsed;

    // append this run to the gantt
    if(elapsed>0){
        sim_clock += elapsed;
        char seg[64];
        snprintf(seg, sizeof(seg), "P%d-(%d)-", task->client_id, sim_clock);
        if(strlen(gantt_buf) + strlen(seg) < sizeof(gantt_buf) - 1){
            strcat(gantt_buf, seg);
        }
    }

    // check waitpid in case child exited without us seeing EOF
    if(!finished && !preempted){
        int st;
        pid_t r = waitpid(task->pid, &st, WNOHANG);
        if(r == task->pid) finished = 1;
    }

    if(finished || task->n_remaining <= 0){
        // drain anything left in the pipe
        char rbuf[256];
        int r;
        while((r = read(task->stdout_fd, rbuf, sizeof(rbuf))) > 0){
            append_output(task, rbuf, r);
        }
        // reap the child if it isnt already
        int st;
        waitpid(task->pid, &st, 0);
        close(task->stdout_fd);
        task->stdout_fd = -1;
        task->pid = -1;
        return 1;
    }

    // pause the child while it waits in the queue
    kill(task->pid, SIGSTOP);
    return 0;
}

static void send_program_output(Task *task){
    int len = task->output_len;
    send(task->client_socket, &len, sizeof(int), 0);
    if(len > 0) send(task->client_socket, task->output, len, 0);
    log_bytes_sent(task->client_id, len);
    log_state(task->client_id, "ended", 0);
}


// scheduler thread main loop

static void *scheduler_thread_func(void *arg){
    (void)arg;

    while(1){
        // block until a task is ready
        sem_wait(&tasks_available);

        pthread_mutex_lock(&queue_lock);
        Task *task = pick_next_task();
        if(task == NULL){
            // queue got emptied (e.g. client disconnect) before we got the lock
            pthread_mutex_unlock(&queue_lock);
            continue;
        }
        remove_locked(task);
        running_task = task;
        pthread_mutex_unlock(&queue_lock);

        // 'started' on first round, 'running' after that
        if(task->rounds_done == 0)
            log_state(task->client_id, "started", task->n_remaining);
        else
            log_state(task->client_id, "running", task->n_remaining);
        task->state = ST_RUNNING;

        if(task->type == TASK_SHELL){
            run_shell_task(task);

            pthread_mutex_lock(&queue_lock);
            running_task = NULL;
            if(last_run_task == task) last_run_task = NULL;
            pthread_mutex_unlock(&queue_lock);

            free_task(task);
            continue;
        }

        // program task: resume existing child or spawn a new one
        if(task->pid > 0){
            kill(task->pid, SIGCONT);
        }
        else if(spawn_program(task) < 0){
            pthread_mutex_lock(&queue_lock);
            running_task = NULL;
            pthread_mutex_unlock(&queue_lock);
            free_task(task);
            continue;
        }

        int finished = run_program_quantum(task);

        pthread_mutex_lock(&queue_lock);
        running_task = NULL;
        last_run_task = task;
        pthread_mutex_unlock(&queue_lock);

        if(finished){
            send_program_output(task);

            pthread_mutex_lock(&queue_lock);
            if(last_run_task == task) last_run_task = NULL;
            int queue_empty = (queue_head == NULL);
            pthread_mutex_unlock(&queue_lock);

            if(queue_empty) log_gantt();

            free_task(task);
        }
        else{
            task->rounds_done++;
            task->state = ST_WAITING;
            log_state(task->client_id, "waiting", task->n_remaining);

            pthread_mutex_lock(&queue_lock);
            enqueue_locked(task);
            pthread_mutex_unlock(&queue_lock);
            sem_post(&tasks_available);
        }
    }

    return NULL;
}


// public api

void scheduler_init(){
    if(sem_init(&tasks_available, 0, 0) != 0){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
    if(pipe(preempt_pipe) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // non-blocking so the drain loop never stalls on an empty pipe
    int fl = fcntl(preempt_pipe[0], F_GETFL, 0);
    fcntl(preempt_pipe[0], F_SETFL, fl | O_NONBLOCK);
}

void scheduler_start(){
    pthread_t th;
    if(pthread_create(&th, NULL, scheduler_thread_func, NULL) != 0){
        error_thread_creation();
        exit(EXIT_FAILURE);
    }
    pthread_detach(th);
}

int scheduler_enqueue(int client_id, int client_socket, const char *command){
    Task *t = malloc(sizeof(Task));
    if(t == NULL){
        error_malloc_failed();
        return -1;
    }

    classify_command(command, &t->type, &t->n_total);

    t->client_id     = client_id;
    t->client_socket = client_socket;
    strncpy(t->command, command, MAX_INPUT - 1);
    t->command[MAX_INPUT - 1] = '\0';
    t->n_remaining = t->n_total;
    t->rounds_done = 0;
    t->state       = ST_CREATED;
    t->pid         = -1;
    t->stdout_fd   = -1;
    t->output      = NULL;
    t->output_len  = 0;
    t->next        = NULL;

    pthread_mutex_lock(&queue_lock);
    t->arrival_seq = ++arrival_counter;
    enqueue_locked(t);

    // decide preemption while we still hold the lock so running_task is stable
    int should_preempt = 0;
    if(running_task != NULL){
        // shells always preempt (they have priority and never get preempted themselves)
        if(t->type == TASK_SHELL){
            should_preempt = 1;
        }
        else if(running_task->type == TASK_PROGRAM &&
                t->n_remaining < running_task->n_remaining){
            should_preempt = 1;
        }
    }
    pthread_mutex_unlock(&queue_lock);

    log_state(t->client_id, "created", t->n_total);

    sem_post(&tasks_available);
    if(should_preempt) write(preempt_pipe[1], "x", 1);
    return 0;
}

void scheduler_remove_client(int client_socket){
    pthread_mutex_lock(&queue_lock);

    // drop every queued task that belongs to this client
    Task **pp = &queue_head;
    while(*pp != NULL){
        if((*pp)->client_socket == client_socket){
            Task *dead = *pp;
            *pp = dead->next;
            if(last_run_task == dead) last_run_task = NULL;
            // keep tasks_available consistent with the queue size
            sem_trywait(&tasks_available);
            if(dead->pid > 0) kill(dead->pid, SIGKILL);
            free_task(dead);
        }
        else{
            pp = &(*pp)->next;
        }
    }

    // if the running task is theirs, kill its child, scheduler will see EOF
    if(running_task != NULL && running_task->client_socket == client_socket){
        if(running_task->pid > 0) kill(running_task->pid, SIGKILL);
    }

    pthread_mutex_unlock(&queue_lock);
}
