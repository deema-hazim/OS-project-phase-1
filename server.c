// server.c
// phase 4: client threads just push commands onto the scheduler queue,
// the scheduler thread is the one that actually runs them

#include "myshell.h"
#include "scheduler.h"

#include <signal.h>

// shared with scheduler.c so log lines from different threads dont mix
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

// counter for client ids ([1], [2], ...)
int client_count = 0;

typedef struct {
    int socket;
    int id;
} ClientInfo;


// thread function for one client, just reads commands and enqueues them
void *handle_client(void *arg){
    ClientInfo *client = (ClientInfo *)arg;
    char buf[MAX_INPUT];

    while(1){
        memset(buf, 0, sizeof(buf));

        int bytes = recv(client->socket, buf, sizeof(buf) - 1, 0);
        if(bytes <= 0) break; // client disconnected or recv error

        buf[bytes] = '\0';

        // strip the trailing newline
        size_t len = strlen(buf);
        if(len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
        if(strlen(buf) == 0) continue;

        if(strcmp(buf, "exit") == 0) break;

        pthread_mutex_lock(&print_lock);
        printf("[%d]>>> %s\n", client->id, buf);
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

        scheduler_enqueue(client->id, client->socket, buf);
    }

    // drop any tasks left for this client before closing
    scheduler_remove_client(client->socket);
    close(client->socket);
    free(client);
    return NULL;
}


int main(){
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        error_socket_creation();
        exit(EXIT_FAILURE);
    }

    // allow port reuse
    int value = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if(bind(server_socket,
            (struct sockaddr *) &server_address,
            sizeof(server_address)) < 0){
        error_bind_failed();
        exit(EXIT_FAILURE);
    }

    if(listen(server_socket, 5) < 0){
        error_listen_failed();
        exit(EXIT_FAILURE);
    }

    // dont die if a client closes mid-send
    signal(SIGPIPE, SIG_IGN);

    // start scheduler before accepting connections
    scheduler_init();
    scheduler_start();

    pthread_mutex_lock(&print_lock);
    printf("-------------------------\n");
    printf("| Hello, Server Started |\n");
    printf("-------------------------\n");
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);

    // accept loop
    while(1){
        int client_socket = accept(server_socket,
                                   (struct sockaddr *) &client_address,
                                   &client_len);
        if(client_socket < 0){
            error_accept_failed();
            continue;
        }

        client_count++;

        ClientInfo *info = malloc(sizeof(ClientInfo));
        if(info == NULL){
            error_malloc_failed();
            close(client_socket);
            continue;
        }

        info->socket = client_socket;
        info->id     = client_count;

        pthread_mutex_lock(&print_lock);
        printf("[%d]<<< client connected\n", info->id);
        fflush(stdout);
        pthread_mutex_unlock(&print_lock);

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
