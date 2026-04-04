#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

#define PORT 5100 
void* ThreadRun(void *);

int main(int argc, char const *argv[]) 
{ 
        int server_fd, new_socket; 
        struct sockaddr_in address; 
        int addrlen = sizeof(address); 
        char hello[255];

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
        
        while(1){
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

        char read_str[1024];
        recv(s,read_str,sizeof(read_str),0);     
        printf("%s",read_str);

        close(s);

}
