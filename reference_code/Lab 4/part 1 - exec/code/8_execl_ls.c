#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("Using *execl* to exec ls -l...\n");
    
   //the arguments that you want to pass to the executable followed by NULL.
   //NULL signifies end of the srgument list
    execl("/bin/ls","ls","-l",NULL);   
    printf("Program Terminated\n");
    return 0;
}