#include<stdio.h>
#include<signal.h>
#include<unistd.h>


void InterruptHandler()
{
	printf("I won't die :P :P \n");
}

void TERMhandler()
{
	printf("I can't hear you!  Na, na, na, na ...\n");
}


int main()
{
	while(1)
	{
		//CTRL + C : interrupt
		//if SIGINT encountred, do handler function
		signal(SIGINT , InterruptHandler);

		//test by opening a new terminal window and type "kill -TERM pid"
		signal(SIGTERM , TERMhandler);

		//test by opening a new terminal window and type "kill -KILL pid" or "kill -9 pid"
		//this won't work: SIGKILL is UNSTOPPABLE
		//The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.
		
		signal(SIGKILL, InterruptHandler);


		
		printf("Wasting cycles .. %d\n", getpid());
		sleep(1);
	}
}  