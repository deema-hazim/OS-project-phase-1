#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define MAX_SIZE 100

int main()
{
    int pipe1fds[2];//for pipe 1 : string communication
    int pipe2fds[2];//for pipe 2 : number of characters communciation
    
    pid_t pid;

    //create pipe
    if(pipe(pipe1fds) == -1){
        perror("pipe 1 error");
        exit(EXIT_FAILURE);
    }

     if(pipe(pipe2fds) == -1){
        perror("pipe 2 error");
        exit(EXIT_FAILURE);
    }

    

    pid = fork();

    //CHILD
    if (pid == 0)
    {
      //will write to pipe 1 so close reading-end of pipe 1
      close(pipe1fds[0]);

      //will read from pipe 2 so close writing-end to pipe 2
      close(pipe2fds[1]);
      
      int n = 0;
      char input[MAX_SIZE];

      printf("Enter a string\n");
      
      // scan a string from the user
      if(fgets(input, sizeof(input), stdin) == NULL)  
        printf("%s\n", "Error while reading input from user");
     
      else {
        // remove trailing new line
        strtok(input, "\n"); 

        // returns the number of bytes read
        //write string to pipe 1
        n = write(pipe1fds[1], input, strlen(input));
              
        if(n < 0)
          perror("Error in writing");

        printf("%s %s\n", "Child wrote", input);
        printf("Child wrote %d bytes\n", n);
      }

      //done writing string to pipe 1, so close writing end to pipe 1
      close(pipe1fds[1]);
      
      int aReceived;

      //read from pipe2
      read(pipe2fds[0], &aReceived, sizeof(int));
      
      //done with reading so close reading end of pipe 2
      close(pipe2fds[0]);
      
      printf("Child: Length received from parent=%d \n", aReceived);
      printf("Child: Length transmitted=%ld \n", strlen(input));
      
      if (strlen(input)==aReceived)
        printf("Message successfully received!!\n");
      else{
        printf("Message Transmission Failed!!\n");
        exit(EXIT_FAILURE);
      }
    }

    //PARENT
    else if (pid > 0)
    {
      //will read string from pipe 1 so close writing-end to pipe 1
      close(pipe1fds[1]);
      
      //will write string size to pipe 2 so close reading-end of pipe 2
      close(pipe2fds[0]);
      
      char buff[100];
      int n = 0;

      //read string from pipe1
      n = read(pipe1fds[0], buff, sizeof(buff));

      if(n < 0)
        perror("Error in reading");
      
      else {
        buff[n] = '\0'; // discard any gibberish chars
        printf("Parent read %s\n", buff);
        printf("Parent read %d bytes\n", n);
      }

      //done with reading so close reading-end of pipe 1
      close(pipe1fds[0]);
      
      int a=strlen(buff);

      //write size of string to pipe 2
      write(pipe2fds[1], &a, sizeof(int));
      printf("Parent length received=%d \n", a);
      
      //done with writing so close writing-end of pipe 2
      close(pipe2fds[1]);
    }

    else
    {
      printf("Error in forking");
      exit(EXIT_FAILURE);
    }

    return 0;
}