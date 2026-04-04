#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>



int main()
{
    printf("Original process with PID %ld and PPID %ld\n", getpid(), getppid());

pid_t pid = fork();
switch(pid){
case -1:
    printf("error in forking\n");
    exit(1);
    break;
case 0:
    printf("Child process with PID %ld and PPID %ld\n", getpid(), getppid());
    pid_t pid2 = fork();
    if(pid2 < 0) {

    printf("error");
    exit(1);
    }
    else if (pid2 > 0) {

    printf("Child process with PID %ld and PPID %ld\n", getpid(), getppid());
    wait(NULL);
    printf("Child is about to exit\n");
    exit(0);

    }
    else {
    // grandchild

    printf("Grand child process with PID %ld and PPID %ld\n", getpid(), getppid()); printf("Grandchild is about to exit\n"); exit(0);
    }
    break;
default:

printf("Parent process with PID %ld and PPID %ld\n", getpid(), getppid());
wait(NULL);
printf("Parent is about to execute the ps command\n");
char* exp[] = {"ps", NULL};
sleep(2);
execvp("ps", exp);
break;

}
    
    
    return 0;
}



