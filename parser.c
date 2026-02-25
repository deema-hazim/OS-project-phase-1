// parser.c - handles parsing user input into the Pipeline/Command structs
// for now this only handles parts 1 & 2 (simple commands with/without args)
// will be extended later to handle pipes, redirections, etc.

#include "myshell.h"


// sets every field in the pipeline to a clean default state
// called before parse_input so we dont have leftover garbage values
void init_pipeline(Pipeline *p)
{
    int i;
    p->num_commands = 0;

    // loop through all possible command slots and zero them out
    for (i = 0; i < MAX_PIPES; i++) {
        int j;
        // set all arg pointers to NULL
        for (j = 0; j < MAX_ARGS; j++) {
            p->commands[i].args[j] = NULL;
        }
        p->commands[i].input_file = NULL;
        p->commands[i].output_file = NULL;
        p->commands[i].error_file = NULL;
    }
}


// takes the raw input string and fills in the pipeline struct
// for parts 1 & 2 we just tokenize by spaces into the first command slot
// returns 0 on success, -1 if something went wrong
int parse_input(char *input, Pipeline *p)
{
    int i = 0;

    // tokenize the input by spaces and tabs using strtok
    // first call takes the string, next calls use NULL to keep going
    char *token = strtok(input, " \t");

    // if theres nothing to parse, return error
    if (token == NULL) {
        return -1;
    }

    // fill in args array for the first (and only) command
    while (token != NULL && i < MAX_ARGS - 1) {
        p->commands[0].args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }

    // NULL terminate the args array - execvp needs this
    p->commands[0].args[i] = NULL;

    // we have one command in the pipeline
    p->num_commands = 1;

    return 0;
}
