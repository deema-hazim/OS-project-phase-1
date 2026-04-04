#include <stdio.h>
#include <unistd.h>

//execve() uses filename, argument array and provided ENV to execute the program.

//It only takes a filename to invoke the program, and it is searched in paths
//specified in $PATH in sequence.

//The arguments are listed in an array of char* and passed to the function.

//The new sets of environment variables are declared in an array of char*
//and passed to the program in the function. They can be used to change the
//behaviour of the new program, for example, ls in this example.

int main(){ 
    char *env[] = {"LS_COLORS=ow=1;34;45:",NULL}; 
    char *arg[] = {"ls","-l","--color",NULL}; 
    printf("Using *execve* to exec ls -l\n");  
    execve("/bin/ls",arg,env); 
    printf("Program Terminated\n");
    return 0;
}