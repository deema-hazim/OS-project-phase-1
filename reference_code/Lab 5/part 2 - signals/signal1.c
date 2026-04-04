#include<stdio.h>
#include<signal.h>
#include<unistd.h>


int main()
{
	while(1)
	{
		printf("Wasting cycles ... %d\n", getpid());
		sleep(1);
	}
}  


//test different signals by opening a new terminal window and type:

//Ctrl + C : interrupt
//kill -TERM pid
//kill -9 pid or kill -KILL pid
//Ctrl + Z :  kill -STOP pid. Puts process in background. Can resume later.
//kill -CONT pid for stopped processes

