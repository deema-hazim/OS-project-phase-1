#include <stdio.h>
#include <unistd.h>

//execv() uses pathname, argument array and original ENV to execute the program.

//You need to specify the absolute path for your target program and 
//array of char* for the program arguments.

//execv() uses original system enviroment variables for the new program.

int main(){ 
    char *arg[] = {"ls","-l",NULL}; 
    printf("Using *execv* to exec ls -l...\n");
    execv("/bin/ls",arg); 
    printf("Program Terminated\n"); 
    return 0;
}