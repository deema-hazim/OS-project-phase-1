#include "myshell.h"

void init_pipeline(Pipeline *p){
    p->num_commands = 0;

    for (int i = 0; i < MAX_PIPES; i++) {
        // set all redirection filenames to NULL
        p->commands[i].input_file  = NULL;
        p->commands[i].output_file = NULL;
        p->commands[i].error_file  = NULL;

        //set all argument slots to NULL
        for (int j = 0; j < MAX_ARGS; j++) {
            p->commands[i].args[j] = NULL;
        }
    }
}

int parse_input(char *input, Pipeline *p){
    init_pipeline(p); //reset the pipeline struct before parsing new input

    char *segments[MAX_PIPES]; //array to hold pointers to the start of each command segment
    int num_segments = 0; //tracks how many segments we have

    //make a copy of the input so we can modify it safely
    static char input_copy[MAX_INPUT];
    strncpy(input_copy, input, MAX_INPUT - 1);
    input_copy[MAX_INPUT - 1] = '\0'; //manually null-terminate to be safe

    //segments[0] points to the start of the first command
    char *ptr = input_copy;
    segments[num_segments++] = ptr; //sets it to 0 and then increments to 1 for the next segment

    //while we haven't reached the end of the string, look for pipe symbols
    while (*ptr != '\0'){

        //once we reach a pipe symbol, replace it with a null terminator to end the current segment
        if (*ptr == '|'){
            *ptr = '\0'; //terminate current segment
            ptr++; //move past the pipe |

            //check for empty command between pipes
            char *trimmed = ptr;
            while(*trimmed == ' ') trimmed++;

            // empty command between two pipes e.g. "ls | | grep"
            if (*trimmed == '|') {
                error_empty_command_between_pipes();
                return -1;
            }

            // check we haven't exceeded max pipes
            if (num_segments >= MAX_PIPES) {
                fprintf(stderr, "Too many pipes.\n");
                return -1;
            }

            segments[num_segments++] = ptr;
        }

        //if the current character is not a pipe, just keep moving forward until we find one or reach the end
        else ptr++;
    }

    //check for trailing pipe such as cmd1 | with nothing after
    char *last = segments[num_segments - 1];
    while (*last == ' ') last++;
    if (*last == '\0') {
        error_missing_command_after_pipe();
        return -1;
    }

    //store the number of segments (commands) in the pipeline
    p->num_commands = num_segments;

    //loop through each command segment
    for (int i = 0; i < num_segments; i++) {
        Command *cmd = &p->commands[i]; //cmd is a pointer to the current command struct we're filling in
        int arg_index = 0; //tracks how many args we've added so far

        //strtok splits segments[i] by spaces and tabs, returning the first token
        char *token = strtok(segments[i], " \t");

        //keep going until strtok returns NULL
        while (token != NULL){

            //if the current token is "<" we know the next token must be the input filename
            if (strcmp(token, "<") == 0){
                //continue from where strtok left off to get the next token, which should be the filename
                token = strtok(NULL, " \t");
                if (token == NULL){
                    error_missing_input_file();
                    return -1;
                }
                cmd->input_file = token; //store the input filename in the command struct

            }

            //same logic as above but for output redirection
            else if (strcmp(token, ">") == 0){
                token = strtok(NULL, " \t");
                if (token == NULL) {
                    error_missing_output_file();
                    return -1;
                }
                cmd->output_file = token; //store output filename in command struct

            }

            //same logic as above but for error redirection
            else if (strcmp(token, "2>") == 0){
                //continue where strtok left off to get the next token which should be the error filename
                token = strtok(NULL, " \t");
                if (token == NULL) {
                    error_missing_error_file();
                    return -1;
                }
                cmd->error_file = token;

            }

            else{
                // otherwise, it's a regular argument, command name or flag, so add it to the args array
                if (arg_index >= MAX_ARGS - 1) {
                    fprintf(stderr, "Too many arguments.\n");
                    return -1;
                }
                cmd->args[arg_index++] = token;
            }

            //get the next token from this segment and continue the loop
            token = strtok(NULL, " \t");
        }

        //after all the tokens are processed, we need to NULL terminate the args array so execvp() knows where it ends
        cmd->args[arg_index] = NULL;

        //check that this command actually has something to run
        if (cmd->args[0] == NULL) {
            fprintf(stderr, "Missing command.\n");
            return -1;
        }
    }

    return 0;
}