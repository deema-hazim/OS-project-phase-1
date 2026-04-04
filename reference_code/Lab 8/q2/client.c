#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 5100 

int main(int argc, char const *argv[]) 
{ 
        int sock = 0, valread; 
        struct sockaddr_in serv_addr; 
        char buffer[1024] = {0}; 
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        { 
                printf("\n Socket creation error \n"); 
                return -1; 
        } 

        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(PORT); 


        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
                printf("\nConnection Failed \n"); 
                return -1; 
        } 

        valread = read( sock , buffer, 1024); 
        printf("%s\n",buffer ); 
		
	char newstr[1024]={0};
	printf("Client: Enter a string: ");
	fgets(newstr, sizeof(newstr),stdin);
	send(sock, newstr, strlen(newstr),0);
	printf("Message sent\n");

	close(sock);       
	return 0; 
} 

