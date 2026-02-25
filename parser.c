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

// splits the input by spaces/tabs and fills in the pipeline struct
// detects > and 2> for output/error redirection and stores the filenames
// returns 0 if ok, -1 if theres a problem
int parse_input(char *input, Pipeline *p){
    int i = 0;
    char *tok = strtok(input, " \t");

    if(tok == NULL)
        return -1;

    while(tok != NULL && i < MAX_ARGS - 1){

        // check for output redirection >
        if(strcmp(tok, ">") == 0){
            tok = strtok(NULL, " \t");
            if(tok == NULL){
                error_missing_output_file();
                return -1;
            }
            p->commands[0].output_file = tok;
        }
        // check for error redirection 2>
        else if(strcmp(tok, "2>") == 0){
            tok = strtok(NULL, " \t");
            if(tok == NULL){
                error_missing_error_file();
                return -1;
            }
            p->commands[0].error_file = tok;
        }
        // regular arg, just add to the args array
        else{
            p->commands[0].args[i] = tok;
            i++;
        }
        tok = strtok(NULL, " \t");
    }
    p->commands[0].args[i] = NULL; // execvp needs null at end
    p->num_commands = 1;

    return 0;
}
