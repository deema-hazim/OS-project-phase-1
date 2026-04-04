#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>

//compile with -lpthread

void *myThreadFun(void *args)
{
	sleep(1);
	printf("Hello World from Thread \n");
	return NULL;
}

int main()
{
	/*declare a variable called thread, which is of type pthread_t,
	 which is an integer used to identify the thread in the system*/
	pthread_t thread;

	printf("Before Thread\n");
	
	/*pthread_create() : function to create a thread.
	takes 4 arguments:
	1st is a pointer to the thread
	
	2nd specifies attributes for thread. If the value is NULL, then
	default attributes shall be used.
	
	3rd is name of function to be executed for the thread to be 
	created.
	
	4th is used to pass arguments to the function, myThreadFun.
	*/
	pthread_create(&thread, NULL, myThreadFun, NULL);
	
	/*The pthread_join() function for threads is the equivalent
	 of wait() for processes. A call to pthread_join BLOCKS the
	 calling thread until the thread in the 1st argument terminates.
	
	pthread_join is used to link the current thread process to
	another thread. It literally makes the program stop in order
	to wait for the end of the selected thread.

	*/
	pthread_join(thread, NULL);

	printf("After Thread\n");
	exit(0);
}
