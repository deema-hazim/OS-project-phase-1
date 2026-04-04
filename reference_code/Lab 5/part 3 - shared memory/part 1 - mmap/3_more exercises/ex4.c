#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include <string.h>


void print_array(int *array, int N)
{
    for(int i=0;i<N; i++)
        printf("%d ",*(array+i));
    
    printf("\n");
}

int main()
{
  
    int N;
      
    
    int * shared_value= (int *) mmap(NULL, sizeof(int), PROT_READ |PROT_WRITE, MAP_SHARED |MAP_ANONYMOUS, -1,0);
    
    
    if (shared_value== MAP_FAILED)
    {
        perror ("Failed: mmap");
        exit(EXIT_FAILURE);
    }

                
    pid_t pid1=fork();
    
    if (pid1<0)
    {
        printf("Failed Fork pid1");
        exit(1);
    }
    else
    {
        if (pid1==0) //child1
        {
            int i;
            for (i = 0; i < 10000; i++)
                *shared_value = *shared_value + 1;
            printf("%s %d\n","Child 1", *shared_value);
            exit(0); 
        }
        else
        {
            //parent
            waitpid(pid1, NULL, 0); //wait for child1 to complete

            pid_t pid2=fork();
            
            if (pid2 == 0) //child2
            {
                
                for (int i = 0; i < 5000; i++)
                    *shared_value = *shared_value + 1;
                    
                printf("%s %d\n","Child 2", *shared_value);
                exit(0);
            }
            else
            {
                //parent
                //wait for child2 to complete
                waitpid(pid2, NULL, 0);
        
                for (int i = 0; i < 5000; i++)
                    *shared_value = *shared_value + 1;

                printf("%s %d\n","Parent", *shared_value);
             }
         
         }
    }
      

    munmap(shared_value, sizeof(int));
    return 0;
}







