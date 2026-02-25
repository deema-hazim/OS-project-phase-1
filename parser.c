// parser.c
// handles parsing the user input into the Pipeline and Command structs

#include "myshell.h"

// zeros out the whole pipeline struct so nothing has garbage values
void init_pipeline(Pipeline *p){
    p->num_commands = 0;
    int i, j;
    for(i = 0; i < MAX_PIPES; i++){
        for(j = 0; j < MAX_ARGS; j++)
            p->commands[i].args[j] = NULL;
        p->commands[i].input_file = NULL;
        p->commands[i].output_file = NULL;
        p->commands[i].error_file = NULL;
    }
}

// splits the input by spaces/tabs and puts it into the first command slot
// returns 0 if ok, -1 if empty
int parse_input(char *input, Pipeline *p){
    int i = 0;
    char *tok = strtok(input, " \t");

    if(tok == NULL)
        return -1;

    // keep grabbing tokens until end of string or max args
    while(tok != NULL && i < MAX_ARGS - 1){
        p->commands[0].args[i] = tok;
        i++;
        tok = strtok(NULL, " \t");
    }
    p->commands[0].args[i] = NULL; // execvp needs null at end
    p->num_commands = 1;

    return 0;
}
