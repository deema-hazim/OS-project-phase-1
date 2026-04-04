#include <stdio.h>
#include <unistd.h>

//execvp() uses filename, argument array and original ENV to execute the program.

//It only takes a filename to invoke the program, 
//and it is searched in paths specified in $PATH in sequence.

//The PATH variable is just a special variable in bash that contains all of the
//directories that are automatically searched when you try to call a program.

//The arguments are listed in an array of char* and passed to the function.

//Original environment variable are used in a new program environment.

int main()
{
    char *arg[] = {"ls","-l",NULL};
    printf("Using *execvp* to exec ls -l ...\n");
    execvp("ls",arg);
    printf("Program Terminated\n");
    return 0;
}