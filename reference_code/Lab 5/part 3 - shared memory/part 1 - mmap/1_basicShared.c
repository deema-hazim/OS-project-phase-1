#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>


/*
Normally when you clone a process, the parent process and the child process DON'T share memory.
The memory is copied and all changes are only seen by the process that made the change.
So the child makes some changes to memory, the child ONLY sees those changes
The parent makes some changes, the parent sees only the PARENT's changes
BUT they don't see each others' changes
*/



int v = 5;

int main(int argc, char** argv)
{
	if (fork() == 0) //variable v=5 is copied to child process after fork
	{
		//IN CHILD
		v = 80; //changing valiable value
		printf("V == %d\n", v);

	}
	else
	{
		//IN PARENT
		//parent will wait for child to complete
		wait(NULL); 
		printf("V == %d\n", v);
	}

}