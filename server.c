// server.c
// server side of remote shell, receives commands and sends output back

#include "myshell.h"

// captures command output by redirecting stdout/stderr into a pipe
// returns malloc'd buffer, caller frees it
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
        //child - redirect stdout/stderr to pipe then run command
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);

        //use phase 1 code to parse and execute
        Pipeline pl;
        init_pipeline(&pl);
        if(parse_input(cmd_str, &pl) == -1)
            exit(EXIT_FAILURE);
        execute_pipeline(&pl);
        exit(EXIT_SUCCESS);
    }

    //parent - read output from pipe
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
    //read until EOF (child and grandchildren all exited)
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

    waitpid(pid, status, 0);
    output[total] = '\0';
    *out_len = total;
    return output;
}

int main(){
    int server_socket, client_socket;
    struct sockaddr_in server_address;
    int addrlen = sizeof(server_address);
    char buf[MAX_INPUT];

    //create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        printf("socket creation failed..\n");
        exit(EXIT_FAILURE);
    }

    //allow port reuse
    int value = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    //set up address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    //bind
    if(bind(server_socket,
        (struct sockaddr *) &server_address,
        sizeof(server_address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //listen
    if(listen(server_socket, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server started, waiting for client connections...\n");

    //accept a client connection
    client_socket = accept(server_socket,
        (struct sockaddr *) &server_address,
        (socklen_t *) &addrlen);

    if(client_socket < 0){
        perror("accept failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Client connected.\n");

    //command loop
    while(1){
        memset(buf, 0, sizeof(buf));

        int bytes = recv(client_socket, buf, sizeof(buf) - 1, 0);

        //client disconnected or error
        if(bytes <= 0){
            if(bytes == 0)
                printf("[INFO] Client disconnected.\n");
            else
                perror("recv");
            break;
        }

        buf[bytes] = '\0';

        //strip newline
        size_t len = strlen(buf);
        if(len > 0 && buf[len-1] == '\n')
            buf[len-1] = '\0';

        if(strlen(buf) == 0)
            continue;

        //exit
        if(strcmp(buf, "exit") == 0){
            printf("[INFO] Client requested exit.\n");
            break;
        }

        printf("[RECEIVED] Received command: \"%s\" from client.\n", buf);
        printf("[EXECUTING] Executing command: \"%s\"\n", buf);

        //execute and capture output
        int out_len = 0;
        int child_status = 0;
        char *output = run_and_capture(buf, &out_len, &child_status);

        //if capture failed send error
        if(output == NULL){
            char *err = "Server error: failed to execute command\n";
            int elen = strlen(err);
            send(client_socket, &elen, sizeof(int), 0);
            send(client_socket, err, elen, 0);
            continue;
        }

        //log result, check if command failed
        if(WIFEXITED(child_status) && WEXITSTATUS(child_status) != 0){
            printf("[ERROR] %s", out_len > 0 ? output : "Command failed\n");
            if(out_len > 0 && output[out_len - 1] != '\n')
                printf("\n");
            printf("[OUTPUT] Sending error message to client.\n");
        }
        else{
            if(out_len > 0){
                printf("[OUTPUT] Sending output to client:\n%s", output);
                if(output[out_len - 1] != '\n')
                    printf("\n");
            }
            else{
                printf("[OUTPUT] Command produced no output.\n");
            }
        }

        //send length first then the data
        send(client_socket, &out_len, sizeof(int), 0);
        if(out_len > 0)
            send(client_socket, output, out_len, 0);

        free(output);
    }

    //cleanup
    close(client_socket);
    close(server_socket);
    printf("[INFO] Server shutting down.\n");

    return 0;
}
