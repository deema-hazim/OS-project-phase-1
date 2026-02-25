// executor.c
// forks a child process and uses execvp to run commands

#include "myshell.h"

// checks how many commands and decides what to do
// right now just handles single commands
void execute_pipeline(Pipeline *p){
    if(p->num_commands == 1)
        execute_single(&p->commands[0]);
}

// forks, child calls execvp, parent waits
void execute_single(Command *cmd){
    pid_t pid = fork();

    if(pid < 0){
        perror("fork failed");
        return;
    }
    else if(pid == 0){
        // child - replace with the command
        execvp(cmd->args[0], cmd->args);
        // only gets here if command doesnt exist
        error_command_not_found(cmd->args[0]);
        exit(EXIT_FAILURE);
    }
    else{
        // parent waits for child
        int status;
        waitpid(pid, &status, 0);
    }
}
