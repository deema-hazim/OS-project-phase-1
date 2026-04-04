#include <stdio.h>
#include <unistd.h> // fork()

int main()
{
	fork();
	printf("Hello World\n");
	// printf("Hello World. %d\n", getpid());

}