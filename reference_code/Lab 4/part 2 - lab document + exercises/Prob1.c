#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>



int main()
{
    printf("Main Process PID= %d, PPID=%d\n", getpid(), getppid());
    
    int status;
    pid_t child=fork();
    
    if (child<0)
    {
        // error in fork
        //printf("Failed Fork");
        perror("Failed Fork");
        exit(1);
    }
    else
    {
        if(child==0)
        {
            //Child
            printf("Child Process PID= %d, PPID=%d\n", getpid(), getppid());
            execlp("date", "date", NULL);
            perror("Execlp Failed");
            exit(1);
        }
        else
        {
            //Parent
            wait(&status);
            int exit_status= WEXITSTATUS(status);
            printf("Parent Process PID= %d, child PID=%d, PPID=%d\n", getpid(), child, getppid());
            
            printf("Child exit code= %d\n", exit_status);
            
            if (status != 0) // or exit_status!=0
            {
                        printf("Child Failed %d \n", status);
                }
                else
                {
                    printf("Child succeeded!");
                }
                
        }
    
    }
    
    
    return 0;
}



