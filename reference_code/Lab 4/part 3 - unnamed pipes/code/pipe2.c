/* 
The previous example (pipe1.c) shows the basic operation on pipe.
But it is not very useful for a single process to use a pipe to 
talk to itself. In typical use, a process creates a pipe just
before it forks one or more child processes. 

The pipe is then used for communication either between the parent
or child processes, or between two sibling processes. 

In this example, parent write "OS Pipes" to the pipe, 
child read from the pipe 1 byte at a time until the pipe is empty.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#define SIZE 8

int main()
{
    int pipefds[2];
    pid_t pid;
    char buf[30];


    //create pipe
    if(pipe(pipefds) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //fill a block of memory with given/particular value.
    //fills the 30 blocks of buf with 0
      memset(buf,0,30);

      pid = fork();

    //PARENT 
    if (pid > 0) 
    {
      printf("PARENT write in pipe\n");

    //parent close the read end
      close(pipefds[0]);

    //parent write in the pipe write end                 
      write(pipefds[1], "OS Pipes", SIZE);

    //after finishing writing, parent close the write end
      close(pipefds[1]);

    //parent wait for child                
      wait(NULL); 

    }
    //CHILD
    else {

      //child close the write end  
      close(pipefds[1]);   

      //child read from the pipe read end until the pipe is empty   
      //read data 1 byte at a time
      while(read(pipefds[0], buf, 1)==1)   
        printf("CHILD read from pipe -- %s\n", buf);

      //after finishing reading, child close the read end
      close(pipefds[0]);
      printf("CHILD: EXITING!\n");
      exit(EXIT_SUCCESS);

    }

/*
After fork(), pipe file descriptors are shared between the
parent and child.
(A child process inherits its parent's file descriptors)

To ensure pipe works properly, you should: 
*Always be sure to close the end of pipe you aren't concerned with.
 That is, if the parent wants to receive data from the child, 
 it should close pipefds[1], and the child should close pipefds[0].

When processes finish reading or writing, close related
file descriptors. Otherwise, there will be undesired 
synchronization problems.
*/
    return 0;
}