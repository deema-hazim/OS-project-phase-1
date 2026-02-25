// main.c - entry point for myshell
// runs the main loop: shows $ prompt, reads input, parses it, executes it
// typing "exit" or Ctrl+D quits the shell

#include "myshell.h"

int main()
{
    char buf[MAX_INPUT];  // buffer for user input
    Pipeline pipeline;    // struct to hold parsed command(s)

    // keep looping until user types exit or Ctrl+D
    while (1) {
        // print prompt and flush so it shows up right away
        printf("$ ");
        fflush(stdout);

        // read a line from user, fgets returns NULL on EOF (ctrl+d)
        if (fgets(buf, MAX_INPUT, stdin) == NULL) {
            printf("\n");
            break;
        }

        // remove the trailing newline that fgets adds
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // skip if user just hit enter
        if (strlen(buf) == 0) {
            continue;
        }

        // built-in exit command
        if (strcmp(buf, "exit") == 0) {
            break;
        }

        // initialize pipeline to clean state before parsing
        init_pipeline(&pipeline);

        // parse the input into the pipeline struct
        // if parsing fails (returns -1), skip to next prompt
        if (parse_input(buf, &pipeline) == -1) {
            continue;
        }

        // execute whatever was parsed
        execute_pipeline(&pipeline);
    }

    return EXIT_SUCCESS;
}
