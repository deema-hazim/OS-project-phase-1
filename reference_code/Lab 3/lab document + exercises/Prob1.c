#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>



int main()
{
    printf("Main Process PID= %d, PPID=%d\n", getpid(), getppid());
    
    int status;
    pid_t pid;

    switch (pid = fork()) {
      case -1:
        // On error fork() returns -1.
        perror("fork failed");
        exit(EXIT_FAILURE);

      case 0:
        // On success fork() returns 0 in the child.
        printf("Child Process PID= %d, PPID=%d\n", getpid(), getppid());
        exit(EXIT_SUCCESS);

      default:
        // On success fork() returns the pid of the child to the parent.
        wait(&status);
        int exit_status= WEXITSTATUS(status);
        
        printf("Parent Process PID= %d, Child PID=%d, PPID=%d\n", getpid(), pid, getppid());
        
        printf("Child exit code= %d\n", exit_status);
        
        if (status != 0) // or exit_status!=0
            printf("Child Failed %d \n", status);
        else
            printf("Child succeeded!\n");
  }

    
    return 0;
}



