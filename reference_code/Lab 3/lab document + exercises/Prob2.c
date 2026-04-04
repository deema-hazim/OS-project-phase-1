#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>

int main()
{
    pid_t child1=fork();
    
    if (child1<0)
    {
        perror("Failed Fork");
        exit(EXIT_FAILURE);
    }
    else
    {
        if(child1==0)
        {
            //Child
            pid_t child3= fork();
            
            if (child3<0)
            {
                printf("Failed Fork Child3");
                exit(EXIT_FAILURE);
            }
            else
            {
                if (child3==0)
                    printf("Child3 Process PID= %d, PPID=%d\n", getpid(), getppid());
                else
                {
                    //child3's parent which is child1
                    wait(NULL);
                    printf("Child1 Process PID= %d, PPID=%d\n", getpid(), getppid());
                }
            }
            
            
        }
        else
        {
            //Parent
            wait(NULL);
            
            pid_t child2= fork();
            
            if (child2<0)
            {
                printf("Failed Fork Child2");
                exit(EXIT_FAILURE);
            }
            else
            {
                if (child2==0)
                    printf("Child2 Process PID= %d, PPID=%d\n", getpid(), getppid());
                else
                {
                    wait(NULL);
                    printf("Parent Process PID= %d, PPID=%d\n", getpid(), getppid());
                }
            }
                
        }
    
    }
    
    
    return 0;
}



