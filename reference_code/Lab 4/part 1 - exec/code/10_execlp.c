#include <stdio.h>
#include <unistd.h>

//execlp() uses filename, argument list and 
//original ENV to execute the program.

//we only need to provide the name of the program, 
//it will search accordingly

//For the program arguments, they are included directly into execlp() functions.
//NULL at the end of the arguments to signify END of arguments


//default environment variables in systm are used.

int main()
{ 
    printf("Using *execlp* exec ls -l...\n");         
    execlp("ls","ls","-l",NULL); 
    printf("Program Terminated\n"); 
    return 0;
}