#include <stdio.h>
#include <unistd.h>


//execlp() uses pathname, argument list 
//and provided ENV to execute the program.

//You need to provide the exact location (pathname) in order to execute it.

//For the arguments, they are specified directly in the execle() function.


//You can define new environment variables in the function. 
//The environment variables are in a char* array. 
//In this example, the behaviour of ls can be changed by the variables. 
//In this case, the colors for displaying directories are changed.


int main()
{ 
	//try ls -l first before execution
	
    char *env[] = {"LS_COLORS=ow=1;34;45:",NULL}; 
    printf("Using *execle* to exec ls -l\n"); 
    execle("/bin/ls","ls","-l","--color",NULL,env); 
    printf("Program Terminated\n");
    return 0;
}