
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

void signalHandler(int signal)
{
	printf("Caught signal %d!\n",signal);
	if (signal==SIGCHLD) {
		printf("Child ended\n");
		wait(NULL);
	}
}

int main()
{
	//SIGUSR1 and SIGUSR2 are user defined functions. They can mean whatever we want them to mean.
	signal(SIGUSR1,signalHandler);

	//When a child process stops or terminates, SIGCHLD is sent to the parent process.
	signal(SIGCHLD,signalHandler);

	if (fork() == 0) {
		printf("Child running...\n");
		sleep(2);
		printf("Child sending SIGUSR1...\n");
		kill(getppid(),SIGUSR1); /*send SIGUSR1 signal to parent*/
		sleep(5);
		printf("Child exitting...\n");  
		exit(0);//a SIGCHLD is sent automatically
	}

	
 	printf("Parent running, PID=%d. Press ENTER to exit.\n",getpid());
	getchar();
	printf("Parent exitting...\n");
	return 0;
 	



}