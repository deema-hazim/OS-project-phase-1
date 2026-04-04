#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>

#define MAX_INPUT_SIZE 256  // Defines the maximum size for the input buffer
#define PORT 5100  // Defines the port number on which the server will listen for incoming client connections

// Function executed by each thread to handle communication with a specific client
void* ThreadFunction(void *client_socket);

int main(int argc, char *argv[])
{
    int sockfd, newsockfd;  // sockfd is the main server socket, newsockfd is the socket for each connected client
    struct sockaddr_in server_address, client_address;  // Structures for storing server and client address information
    socklen_t clientlen;  // Length of the client address structure, used when accepting new connections

    // Create a new socket using IPv4 (AF_INET) and TCP (SOCK_STREAM)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)  // Check if socket creation was successful
    {
        fprintf(stderr, "ERROR opening socket\n");  // If socket creation fails, print an error message
        exit(1);  // Exit the program
    }

    // Allow the reuse of the server address (important when restarting the server quickly after a crash or shutdown to avoid binding issues)
    int value = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));

    // Initialize the server address structure to zero and set its properties
    server_address.sin_family = AF_INET;  // Use IPv4 addresses
    server_address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address
    server_address.sin_port = htons(PORT);  // Set the port number (convert to network byte order using htons)

    // Bind the server socket to the specified IP address and port
    if (bind(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "ERROR on binding\n");  // If binding fails, print an error message
        exit(1);  // Exit the program
    }

    // Start listening for incoming connections, with a backlog of 5 (maximum number of pending connections in the queue)
    listen(sockfd, 5);
    clientlen = sizeof(client_address);  // Set the length of the client address structure

    // Main server loop that continuously accepts new client connections
    while (1)
    {
        // Accept a new connection from a client
        newsockfd = accept(sockfd, (struct sockaddr *)&client_address, &clientlen);
        if (newsockfd < 0)  // Check if the connection was successfully accepted
        {
            fprintf(stderr, "ERROR on accept\n");  // If accept fails, print an error message and continue to the next iteration
            continue;
        }

        // Print a message to indicate that a new client has connected
        printf("Connection accepted from client\n");

        // Create a new thread to handle communication with the connected client
        pthread_t thread;  // Declare a thread identifier
        pthread_create(&thread, NULL, ThreadFunction, (void *)&newsockfd);  // Start the thread and pass the client's socket
        pthread_detach(thread);  // Detach the thread to allow it to clean up its resources automatically when done
    }

    close(sockfd);  // Close the main server socket (this will never actually be reached in this loop)
    return 0;
}

// Function that will run in a new thread for each connected client
// This function handles all communication with the client
void* ThreadFunction(void *client_socket)
{
    int new_sock = *(int *)client_socket;  // Retrieve the client socket file descriptor
    char buffer[MAX_INPUT_SIZE];  // Buffer to store incoming messages from the client

    // Infinite loop to keep the connection alive until the client disconnects
    while (1)
    {
        // Clear the buffer and receive a message from the client
        bzero(buffer, MAX_INPUT_SIZE);  // Clear the buffer to ensure it's empty before receiving new data
        int bytes_received = recv(new_sock, buffer, sizeof(buffer), 0);  // Receive data from the client socket

        // If the client sends an empty message or disconnects, break the loop
        if (bytes_received <= 0)  // If recv() returns 0, the client has closed the connection
        {
            printf("Client disconnected...\n");  // Inform the server that the client has disconnected
            close(new_sock);  // Close the client socket
            pthread_exit(NULL);  // Terminate the thread cleanly
        }

        // Print the message received from the client (arithmetic expression)
        printf("Client message: %s\n", buffer);

        // Variables to store the two numbers and the operator found in the arithmetic expression
        int num1 = 0, num2 = 0, flag = 0, i, j;

        // Parse the arithmetic expression sent by the client
        // This loop assumes that the message will contain two integers and one operator (e.g., "5+3")
        for (i = 0; i < strlen(buffer) - 1; i++)
        {
            // Check for the arithmetic operator in the message
            if (buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/')
            {
                flag = 1;  // Set flag when an operator is found
                j = i;  // Store the position of the operator for later use
            }
            else if (flag == 0)  // If the flag is 0, accumulate the digits of the first number
            {
                num1 = (10 * num1) + (buffer[i] - '0');
            }
            else  // Once the flag is set (operator found), accumulate the digits of the second number
            {
                num2 = (10 * num2) + (buffer[i] - '0');
            }
        }

        // Perform the appropriate arithmetic operation based on the operator found
        int result;
        if (buffer[j] == '+')
            result = num1 + num2;
        else if (buffer[j] == '-')
            result = num1 - num2;
        else if (buffer[j] == '/')
            result = num1 / num2;
        else if (buffer[j] == '*')
            result = num1 * num2;

        // Print the result to the server console
        printf("Sending reply to client: %d\n", result);

        // Send the result back to the client
        bzero(buffer, MAX_INPUT_SIZE);  // Clear the buffer before sending the result
        sprintf(buffer, "%d", result);  // Convert the result to a string
        send(new_sock, buffer, strlen(buffer), 0);  // Send the result back to the client
    }

    return NULL;  // Terminate the thread
}
