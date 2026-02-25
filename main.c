// main.c
// this is the main file for myshell, it runs the shell loop

#include "myshell.h"

int main(){
    char buf[MAX_INPUT];
    Pipeline pl;

    while(1){
        printf("$ ");
        fflush(stdout); // flush because no newline after $

        // read input, if ctrl+d then break
        if(fgets(buf, MAX_INPUT, stdin) == NULL){
            printf("\n");
            break;
        }

        // get rid of newline at end from fgets
        size_t len = strlen(buf);
        if(len > 0 && buf[len-1] == '\n')
            buf[len-1] = '\0';

        // skip empty lines
        if(strlen(buf) == 0)
            continue;

        // exit command
        if(strcmp(buf, "exit") == 0)
            break;

        // init pipeline then parse and execute
        init_pipeline(&pl);
        if(parse_input(buf, &pl) == -1)
            continue;
        execute_pipeline(&pl);
    }

    return 0;
}
