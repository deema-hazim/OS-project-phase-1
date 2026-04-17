// server.c, multithreaded server, so now each client gets its own thread

#include "myshell.h"

// mutex to protect printf calls so output from different threads doesnt mix
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

// tracks how many clients have connected
int client_count = 0;

// this struct holds info about each connected client
typedef struct {
    int socket;
    int id;
    char ip[INET_ADDRSTRLEN];
    int port;
} ClientInfo;


char *run_and_capture(char *cmd_str, int *out_len, int *status){
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
        // child closes read end, redirects to pipe
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);

        // parse the command
        Pipeline pl;
        init_pipeline(&pl);
        if(parse_input(cmd_str, &pl) == -1)
            exit(EXIT_FAILURE);

        if(pl.num_commands == 1){
            // exec directly
            apply_redirections(&pl.commands[0]);
            execvp(pl.commands[0].args[0], pl.commands[0].args);
            error_command_not_found(pl.commands[0].args[0]);
            exit(EXIT_FAILURE);
        }
        else{
            // pipeline
            execute_pipes(&pl);
            exit(EXIT_SUCCESS);
        }
    }

    // parent closes write end, reads all output from pipe
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
                perror("realloc");
                free(output);
                close(fd[0]);
                waitpid(pid, status, 0);
                return NULL;
            }
            output = tmp;
        }
    }
    close(fd[0]);

    // wait for child and collect exit status
    waitpid(pid, status, 0);
    output[total] = '\0';
    *out_len = total;
    return output;
}


// thread function handles one client's session
void *handle_client(void *arg){
    ClientInfo *client = (ClientInfo *)arg;
    char buf[MAX_INPUT];

    // command loop
    while(1){
        memset(buf, 0, sizeof(buf));

        int bytes = recv(client->socket, buf, sizeof(buf) - 1, 0);

        // client disconnected or recv error
        if(bytes <= 0){
            pthread_mutex_lock(&print_lock);
            if(bytes == 0)
                printf("[INFO] Client #%d disconnected.\n", client->id);
            else
                perror("recv");
            pthread_mutex_unlock(&print_lock);
            break;
        }

        buf[bytes] = '\0';

        // strip the newline
        size_t len = strlen(buf);
        if(len > 0 && buf[len-1] == '\n')
            buf[len-1] = '\0';

        if(strlen(buf) == 0)
            continue;

        // handle exit
        if(strcmp(buf, "exit") == 0){
            pthread_mutex_lock(&print_lock);
            printf("\n[RECEIVED] [Client #%d - %s:%d] Received command: \"exit\"\n",
                client->id, client->ip, client->port);
            printf("[INFO] [Client #%d - %s:%d] Client requested disconnect. Closing connection.\n",
                client->id, client->ip, client->port);
            printf("[INFO] Client #%d disconnected.\n", client->id);
            pthread_mutex_unlock(&print_lock);
            break;
        }

        // log the received command
        pthread_mutex_lock(&print_lock);
        printf("\n[RECEIVED] [Client #%d - %s:%d] Received command: \"%s\"\n",
            client->id, client->ip, client->port, buf);
        printf("[EXECUTING] [Client #%d - %s:%d] Executing command: \"%s\"\n",
            client->id, client->ip, client->port, buf);
        pthread_mutex_unlock(&print_lock);

        // execute and capture the output
        int out_len = 0;
        int child_status = 0;
        char *output = run_and_capture(buf, &out_len, &child_status);

        // if the capture failed, send the error to the client
        if(output == NULL){
            char *err = "Server error: failed to execute command\n";
            int elen = strlen(err);
            send(client->socket, &elen, sizeof(int), 0);
            send(client->socket, err, elen, 0);
            continue;
        }

        // display_len strips the  newline
        int display_len = out_len;
        if(display_len > 0 && output[display_len - 1] == '\n')
            display_len--;

        // log the result
        pthread_mutex_lock(&print_lock);
        if(WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0){
            // command failed
            char *cnf = "Command not found: ";
            int cnf_len = strlen(cnf);
            if(strncmp(output, cnf, cnf_len) == 0){
                printf("[ERROR] [Client #%d - %s:%d] Command not found: \"%.*s\"\n",
                    client->id, client->ip, client->port,
                    display_len - cnf_len, output + cnf_len);
            }
            else{
                printf("[ERROR] [Client #%d - %s:%d] %.*s\n",
                    client->id, client->ip, client->port,
                    display_len, output);
            }
            printf("[OUTPUT] [Client #%d - %s:%d] Sending error message to client:\n\"%.*s\"\n",
                client->id, client->ip, client->port,
                display_len, output);
        }
        else{
            if(out_len > 0){
                printf("[OUTPUT] [Client #%d - %s:%d] Sending output to client:\n%s",
                    client->id, client->ip, client->port, output);
                if(output[out_len - 1] != '\n')
                    printf("\n");
            }
            else{
                printf("[OUTPUT] [Client #%d - %s:%d] Command produced no output.\n",
                    client->id, client->ip, client->port);
            }
        }
        pthread_mutex_unlock(&print_lock);

        // send length then data back to client
        send(client->socket, &out_len, sizeof(int), 0);
        if(out_len > 0)
            send(client->socket, output, out_len, 0);

        free(output);
    }

    // cleanup
    close(client->socket);
    free(client);
    return NULL;
}


int main(){
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        printf("socket creation failed..\n");
        exit(EXIT_FAILURE);
    }

    // allow port reuse
    int value = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    // set up the address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind the socket to the address
    if(bind(server_socket,
        (struct sockaddr *) &server_address,
        sizeof(server_address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if(listen(server_socket, 5) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server started, waiting for client connections...\n");

    // accept loop
    while(1){
        int client_socket = accept(server_socket,
            (struct sockaddr *) &client_address,
            &client_len);

        if(client_socket < 0){
            perror("accept failed");
            continue;
        }

        // get the client IP and port from the sockaddr struct
        char *client_ip = inet_ntoa(client_address.sin_addr);
        int client_port = ntohs(client_address.sin_port);

        client_count++;

        // allocate memory for ClientInfo so the thread gets its own copy
        ClientInfo *info = malloc(sizeof(ClientInfo));
        if(info == NULL){
            perror("malloc");
            close(client_socket);
            continue;
        }

        info->socket = client_socket;
        info->id = client_count;
        strncpy(info->ip, client_ip, INET_ADDRSTRLEN);
        info->port = client_port;

        pthread_mutex_lock(&print_lock);
        printf("[INFO] Client #%d connected from %s:%d. Assigned to Thread-%d.\n",
            info->id, info->ip, info->port, info->id);
        pthread_mutex_unlock(&print_lock);

        // create thread for this client
        pthread_t thread;
        if(pthread_create(&thread, NULL, handle_client, (void *)info) != 0){
            error_thread_creation();
            close(client_socket);
            free(info);
            continue;
        }

        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}