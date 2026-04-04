#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h>
#define SHMSIZE 1024 //size of shared memory

int main() 
{ 
    int shmid;
    key_t key;
    char *shm;

    //We need the segment with key 9999 that was created by the writer process
    key = 9999; 

    //Locate the shared memory segment
    shmid = shmget(key, SHMSIZE , 0666);

    //check for faiure (no segment found with that key)
    if (shmid < 0)
    {
        perror("shmget failure");
        exit(1);
    }

    //Attach the segment to the data space
    shm = shmat(shmid , NULL , 0);

    //check for failure
    if (shm == (char *) -1)
    {
        perror("shmat failure");
        exit(1);
    }
  
    printf("Data read from memory: %s\n",shm); 
      
    //detach from shared memory  
    shmdt(shm); 
    
    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 
     
    return 0; 
} 
