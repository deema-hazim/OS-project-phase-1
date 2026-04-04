/*
This example implements shell command "ls | less" in C.

less command is a linux utility which can be used to read 
contents of text file one page (one screen) per time. 
It has faster access because if file is large, 
it don’t access complete file, but access it page by page.

Try it out before reading this code file.
*/



#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
  int pipefds[2];
   pid_t pid;

  if(pipe(pipefds) == -1){
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  pid = fork();

  if(pid == -1)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  //PARENT process - handling the ls command
  /*
ls usually outputs result to STDOUT with file descriptor 1
Now, we want to have the result of ls as input (WRITE) to the pipe INSTEAD of stdout,
so use dup2
*/
  if(pid == 0)
  {
  //replace stdout with the write end of the pipe
    dup2(pipefds[1],STDOUT_FILENO);  


  //close read to pipe
    close(pipefds[0]);               
    execlp("ls","ls",NULL);
    exit(EXIT_SUCCESS);
  }

  else //CHILD process - handling the less command
/*
less usually takes its inupt from STDIN with file descriptor 0
Now, we want to have the input of less from the READ END of the pipe INSTEAD of stdin,
 so use dup2
*/
  {

  //Replace stdin with the read end of the pipe
        dup2(pipefds[0],STDIN_FILENO);  

  //close write to pipe
        close(pipefds[1]);               
        execlp("less","less",NULL);
        printf("parent is this printed??\n");//why? exec doesn't return!
        exit(EXIT_SUCCESS);
    }   
}

/*
The result of pipe4.c is the same as the result we get 
when we run ls | less in the linux subsystem directly.


The parent uses the pipe for ls output. 

That means it needs to change its standard output file descriptor 
to the writing end of the pipe. 

It does this via dup2 system call and then executes ls. 

The child will use the pipe for less input. 

It changes its standard input file descriptor to the reading end 
of pipe by dup2(pfd[0], 0). 

Then child executes less and the result is sent to standard output.

Thus the output of ls flows into the pipe, 
and the input of less flows in from the pipe. 

This is how we connect the standard output of ls 
to the standard input of less.
*/
