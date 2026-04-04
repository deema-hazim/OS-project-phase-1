
//compile with -lpthread

#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//functions in threads have to look a certain way
//they have to return a void pointer 

void* threadFunc(void * tid){
	long * myID = (long *) tid;
	printf("HELLO FROM THREAD %ld\n", *myID);
}




int main()
{
	
	pthread_t thread0;
	pthread_t thread1;
	pthread_t thread2;

	//array for addresses of thread ids
	pthread_t * pthreads[] = {&thread0 , &thread1 , &thread2};

	for (int i = 0 ; i < 3 ; i++)
	{
		pthread_create(pthreads[i], NULL, threadFunc , (void *)pthreads[i]); 

	}
	
	pthread_exit(NULL);

	return 0;
}