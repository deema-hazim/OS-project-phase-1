
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

  //PARENT process - handling the pwd command
  /*
pwd usually outputs result to STDOUT with file descriptor 1
Now, we want to have the result of pwd as input (WRITE) to the pipe INSTEAD of stdout,
so use dup2
*/
  if(pid == 0)
  {
  //replace stdout with the write end of the pipe
    dup2(pipefds[1],STDOUT_FILENO);  


  //close read to pipe
    close(pipefds[0]);               
    execlp("pwd","pwd",NULL);
    exit(EXIT_SUCCESS);
  }

  else //CHILD process - handling the wc command
/*
wc usually takes its inupt from STDIN with file descriptor 0
Now, we want to have the input of wc from the READ END of the pipe INSTEAD of stdin,
 so use dup2
*/
  {

  //Replace stdin with the read end of the pipe
        dup2(pipefds[0],STDIN_FILENO);  

  //close write to pipe
        close(pipefds[1]);               
        execlp("wc","wc","-c",NULL);
        exit(EXIT_SUCCESS);
    }   
}

