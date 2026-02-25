// executor.c - handles forking and executing commands
// for parts 1 & 2: fork a child, run execvp, parent waits

#include "myshell.h"


// decides how to execute based on how many commands are in the pipeline
// if theres just 1 command we run execute_single
// later when we add pipes, multiple commands will go to execute_pipes
void execute_pipeline(Pipeline *p)
{
    // for now we only handle single commands (parts 1 & 2)
    if (p->num_commands == 1) {
        execute_single(&p->commands[0]);
    }
}


// executes a single command (no pipes) by forking a child process
// child runs execvp() which replaces it with the actual program
// parent waits for child to finish before returning
void execute_single(Command *cmd)
{
    pid_t pid = fork();

    if (pid < 0) {
        // fork failed
        perror("fork failed");
        return;
    }
    else if (pid == 0) {
        // child process - run the command
        // execvp searches PATH for the program and runs it
        // cmd->args[0] is the command name, cmd->args is the full array
        execvp(cmd->args[0], cmd->args);

        // if execvp returns it means the command wasnt found
        error_command_not_found(cmd->args[0]);
        exit(EXIT_FAILURE);
    }
    else {
        // parent process - wait for child to finish
        int status;
        waitpid(pid, &status, 0);
    }
}
