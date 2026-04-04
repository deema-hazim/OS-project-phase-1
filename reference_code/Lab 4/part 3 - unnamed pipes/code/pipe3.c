/* 
pipe3.c is almost the same as the previous pipe2.c except 
(close(pipefds[1])) is commented out. 
That means, child forget to close write end. 
Run and compare the output of both files to see what happens.
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

      memset(buf,0,30);
      pid = fork();

    if (pid > 0) 
    {
      printf(" PARENT write in pipe\n");

    //parent close the read end
      close(pipefds[0]);

    //parent write in the pipe write end                 
      write(pipefds[1], "OS Pipes", SIZE);

    //after finishing writing, parent close the write end
      close(pipefds[1]);

    //parent wait for child                
      wait(NULL); 

    }
    else {

      //COMMENTED--NOTICE DIFFERENCE
      // //child close the write end  
      // close(pipefds[1]);   

      //child read from the pipe read end until the pipe is empty   
      while(read(pipefds[0], buf, 1)==1)   
        printf("CHILD read from pipe -- %s\n", buf);

      //after finishing reading, child close the read end
      close(pipefds[0]);
      printf("CHILD: EXITING!");
      exit(EXIT_SUCCESS);

    }

/*
We can see that the child process doesn't exit. 
Why is that?

Note that the child process still has pipefds[1] open.
The system will assume that a write could occur while 
any process has the write end open, even if the only such process 
is the one that is currently trying to read from the pipe, 
and the system will not report EOF. 

Thus child is waiting to read from pipefds[0], 
and the operating system doesn't know that no process will 
be writing to pipdfds[1]. 

So the child process blocks until data arrives to be read.

*/
    return 0;
}