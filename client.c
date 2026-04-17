#include "myshell.h"

int main(){
    //create socket
    //Internet family IPv4, stream socket TCP
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(network_socket == -1){
        error_socket_creation();
        exit(EXIT_FAILURE);
    }
    
    //specify an address for the socket we want to connect to
    struct sockaddr_in server_address;

    //specify address family
    server_address.sin_family = AF_INET;

    //specify the port we want to connect to
    //htons converts the port number to the correct byte order for the network
    server_address.sin_port = htons(PORT);

    //specify the IP address of the server
    //INADDR_ANY connects to the local machine
    server_address.sin_addr.s_addr = INADDR_ANY;

    //connect to the server
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    if (connection_status == -1) {
        error_connection_failed();
        exit(EXIT_FAILURE);
    }

    //main shell loop
    //read a command, send it to the server, receive and print the response
    char command[MAX_INPUT];

    while(1){
        //show the prompt
        printf("$ ");
        fflush(stdout);

        //read input from the user
        if(fgets(command, MAX_INPUT, stdin) == NULL){
            printf("\n");
            break;
        }

        //strip the newline fgets leaves at the end
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n')
            command[len - 1] = '\0';

        //skip empty lines
        if (strlen(command) == 0)
            continue;

        //for the exit command, notify the server then close the connection
        if (strcmp(command, "exit") == 0) {
            send(network_socket, "exit\n", 5, 0);
            printf("Disconnected from server.\n");
            break;
        }

        //send the command to the server
        //append \n as a delimiter so the server knows the command is complete
        strcat(command, "\n");
        send(network_socket, command, strlen(command), 0);

        //receive the length of the output
        //the server sends an int first telling us how many bytes are coming
        int out_len = 0;
        recv(network_socket, &out_len, sizeof(int), 0);

        //receive the actual output if there is any
        if(out_len > 0){
            char *output = malloc(out_len + 1);
            if(output == NULL){
                error_malloc_failed();
                break;
            }

            //keep receiving until we have all out_len bytes
            //recv() might not give us everything in one call
            int total_received = 0;
            while(total_received < out_len){
                int bytes_received = recv(network_socket,
                                          output + total_received,
                                          out_len - total_received, 0);
                if(bytes_received <= 0){
                    error_server_disconnected();
                    free(output);
                    close(network_socket);
                    exit(0);
                }
                total_received += bytes_received;
            }

            output[out_len] = '\0';
            printf("%s", output);
            free(output);
        }
    }

    //close the socket when done
    close(network_socket);

    return 0;
}