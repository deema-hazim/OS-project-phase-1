#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 256  // Defines the maximum size for the input buffer
#define PORT 5100  // Defines the port number for communication

int main(int argc, char *argv[])
{
    int sockfd, n;
    char inputbuf[MAX_INPUT_SIZE];  // Buffer for storing the user input

    // Create a socket for the client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        // Print error and exit if socket creation fails
        fprintf(stderr, "ERROR opening socket\n");
        exit(1);
    }

    // Enable the reuse of address for the socket
    int value = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)); // Set socket options

    // Define the server's address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;  // Use IPv4
    server_address.sin_port = htons(PORT);  // Set the port
    server_address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address

    // Attempt to connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    {
        // Print error and exit if connection fails
        fprintf(stderr, "ERROR connecting\n");
        exit(1);
    }
    printf("Connected to server\n");

    while (1)
    {
        // Prompt the user to enter a message for the server
        printf("Please enter the message to the server: ");
        bzero(inputbuf, MAX_INPUT_SIZE);  // Clear the buffer
        fgets(inputbuf, MAX_INPUT_SIZE - 1, stdin);  // Get the input from the user

        // Send the message to the server
        send(sockfd, inputbuf, strlen(inputbuf), 0);

        // Clear the buffer to receive the server's response
        bzero(inputbuf, MAX_INPUT_SIZE);
        recv(sockfd, inputbuf, (MAX_INPUT_SIZE - 1), 0);  // Receive reply from the server
        printf("Server replied: %s\n", inputbuf);
    }

    return 0;  // Exit the program
}
