#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> // wait()

int main()
{
	//fork a child process 
	pid_t pid;
	pid = fork();
	if (pid < 0) {
	    /* error occurred */
	    return 1;
	} 
	else if (pid == 0) {
	    /* child process */
	    execlp("./hello", "hello", NULL); 
	} 
	else {
	    /* parent process */
	    /* parent will wait for the child to complete */
	    wait(NULL);
	    printf("Child Complete\n");
	}
	return 0;
}