#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 9002 
int main(int argc, char const *argv[]) 
{ 
        int server_fd, new_socket, valread; 
        struct sockaddr_in address; 

        int addrlen = sizeof(address); 
        char hello[255];

        // Creating socket file descriptor 
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        { 
                perror("socket failed"); 
                exit(EXIT_FAILURE); 
        } 

        address.sin_family = AF_INET; 
        address.sin_addr.s_addr = INADDR_ANY; 
        address.sin_port = htons( PORT ); 

        // Forcefully attaching socket to the port 5100 
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
        { 
                perror("bind failed"); 
                exit(EXIT_FAILURE); 
        } 

        if (listen(server_fd, 3) < 0) 
        { 
                perror("listen"); 
                exit(EXIT_FAILURE); 
        } 
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                                        (socklen_t*)&addrlen))<0) 
        { 
                perror("accept"); 
                exit(EXIT_FAILURE); 
        } 
        
        printf("Enter a string: ");
        fgets(hello, 255,stdin);
        send(new_socket , hello , strlen(hello) , 0 ); 
        close(new_socket);
        
        close(server_fd);
        return 0; 

} 


/*
 The sample server code above has the limitation that it only 
 handles one connection, and then dies. 

 A "real world" server should run indefinitely and should have 
 the capability of handling a number of simultaneous connections, 
 each in its own process/thread. 

 This is typically done by forking off a new process or 
 creating a new thread to handle each new connection. 

 Check next multi server code example for
application using while(1) and multitherading
*/