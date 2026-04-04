//RUN ON REMOTE LINUX SERVER _ WON'T WORK CORRECTLY ON YOUR OS

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <stdlib.h>

int main ( int argc , char ** argv ){
	int id =0, err =0;
	int *mem;
	
	id = shmget ( IPC_PRIVATE ,10 ,0666) ; /* Make shared memory segment */
	
	if (id == -1) 
		perror (" Creation ");
	else 
		printf (" Allocated . %d\n" , id);


	mem = shmat (id , NULL, 0); /* Attach the segment */

	if ((int) mem == -1)  
		perror (" Attachment .");
	else 
		printf (" Attached . Mem contents %d\n" ,*mem );

	*mem = 1; /* Give it initial value */
	
	printf (" Start other process . >"); 

	getchar();
	
	printf ("mem is now %d\n", * mem ); /* Print out new value */

	err = shmctl (id , IPC_RMID , NULL); /* Remove segment */
	
	if (err == -1) 
		perror ("Removal.");
	else 
		printf ("Removed. %d\n", (int)(err));
	
	return 0;
}