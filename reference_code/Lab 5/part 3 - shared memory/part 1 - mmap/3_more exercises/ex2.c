#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include <string.h>
#include <ctype.h>

#define ARRAY_SIZE 3
#define STRING_SIZE 50

int main()
{
    /* allocate shared memory for the whole array */
    char ** shared_array= (char **) mmap(NULL, sizeof(char*) * ARRAY_SIZE, PROT_READ |PROT_WRITE, MAP_SHARED |MAP_ANONYMOUS, -1,0);
    
    
    if (shared_array== MAP_FAILED)
    {
        perror ("Failed: mmap");
        exit(EXIT_FAILURE);
    }
    
    //String is an array of characters, so an array of 3 strings is an array of 3 arrays
    //so we need share memory for the whole array AND shared memory for each element in the array
    
    /* allocate shared memory for each element in the array */
    for (int i=0; i<ARRAY_SIZE;i++)
    {
        shared_array[i]= (char *) mmap(NULL, STRING_SIZE, PROT_READ |PROT_WRITE, MAP_SHARED |MAP_ANONYMOUS, -1,0);
    
    }
                
    pid_t pidFork= fork();
    
    if (pidFork<0)
    {
        perror("Fork Failed");
        exit(EXIT_FAILURE);
 
        
    }
    else
    {
        if(pidFork==0)
        {
            //Child
            printf("From child: \n");
            for (int i=0; i<ARRAY_SIZE;i++)
            {
                printf("Please enter String %d:", i+1);
                fgets(shared_array[i], STRING_SIZE, stdin);
            }
        }
        else
        {
            //Parent
            wait(NULL);
            printf("\nFrom Parent: \n");
            
            for (int i=0; i<ARRAY_SIZE; i++)
            {
                printf("\n*** String %d ***\n",i+1);

                //iterate over all characters in each string shared_array[i]
                for (int j=0;j<strlen(shared_array[i])-1;j++)
                    {
                        if(shared_array[i][j] >= 'a' && shared_array[i][j] <= 'z')
                            shared_array[i][j] = toupper(shared_array[i][j]);
                     }
                
                
                printf("%s", shared_array[i]);
                printf("Length = %ld\n", strlen(shared_array[i])-1);
                 
            }
            
        }
    }
    
    //unmapping
    for (int i =0; i<ARRAY_SIZE; i++)
        munmap(shared_array[i], STRING_SIZE);
 
 
     munmap(shared_array, ARRAY_SIZE * sizeof(char*));

    
    return 0;
}






