#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

//compile using -lpthread
//run multiple clients in several terminal windows

#define PORT 9002 

void* ThreadRun(void *);

int main(int argc, char const *argv[]) 
{ 
        int server_fd, new_socket, valread; 
        struct sockaddr_in address; 

        int addrlen = sizeof(address); 

        // Creating socket file descriptor 
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
        { 
                perror("socket failed"); 
                exit(EXIT_FAILURE); 
        } 

        address.sin_family = AF_INET; 
        address.sin_addr.s_addr = INADDR_ANY; 
        address.sin_port = htons( PORT ); 

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
        { 
                perror("bind failed"); 
                exit(EXIT_FAILURE); 
        } 

        //keep listeninig for incoming connections
        while(1)
        {
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

                /*enhance performance and resource consumption by declaring
                it as a detached thread,
                so there is no need to wait for the thread to terminate
                No other threads are waiting on it
                The server will keep running after the termination of this thread*/
                // pthread_t th;
                // pthread_attr_t attr;
                // pthread_attr_init(&attr);
                // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

                // pthread_create(&th,&attr,ThreadRun,&new_socket);

                
                // alternative way of declaration:
                pthread_t th;
                pthread_create(&th,NULL,ThreadRun,&new_socket);
                
        }
        
        close(server_fd);
        return 0; 

} 

void* ThreadRun (void * socket){
        int *sock=(int*)socket;
        int s=*sock;
        
        char hello[255];
        printf("Enter a string: ");
        fgets(hello, 255,stdin);
        send(s , hello , strlen(hello) , 0 ); 
        close(s);

}
