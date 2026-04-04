#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>   // getpid(), getppid(),fork()
#include <sys/wait.h> // wait()

//the parent will sleep for 10 sec, so it will complete its execution after 10 sec.
//But, Child will finish its execution (DIE) using exit() system call while
//its parent process has gone for sleep.

//since the parent hasn't called wait, child will stay in process table as a zombie

int main()
{
  int pid = fork();

  // Parent process
  if (pid > 0)
  {
    //run ps aux in another terminal window to check process table in both cases
    //when wait is commented and when not commented
    //uncomment wait to reap the process
    //wait(NULL);
    printf("Parent will sleep\n");
    sleep(10);
  }
   
  // Child process
  else
  {
    printf("I am a child\n");
    exit(0);
  }
  
  return 0;

}