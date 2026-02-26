#include "myshell.h"

void execute_pipes(Pipeline *p){
    int n = p->num_commands;

    // create n-1 pipes for n commands
    // pipe i connects command i (writes) to command i+1 (reads)
    int pipe_fds[MAX_PIPES - 1][2];

    for (int i = 0; i < n - 1; i++){
        if (pipe(pipe_fds[i]) == -1){
            perror("pipe");
            return;
        }
    }

    // store all child pids so we can wait for all of them at the end
    pid_t pids[MAX_PIPES];

    //Fork one child per command
    for (int i = 0; i < n; i++){
        pids[i] = fork();

        // check for fork error
        if (pids[i] == -1){
            perror("fork");
            return;
        }

        // CHILD PROCESS
        // fork returns 0 in the child, so this block is executed by each child process
        if (pids[i] == 0){
            //connect stdin to read end of previous pipe
            // skip the first command because it reads from the terminal
            if (i > 0){
                if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) == -1){
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
            }

            //connect stdout to write end of next pipe
            // skip for last command because it writes to terminal
            if (i < n - 1){
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1){
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
            }

            // close all pipe fds in the child
            // even though dup2 wired up stdin/stdout, the original
            // pipe fds are still open. if we don't close them the
            // reader never sees EOF and hangs forever
            for (int j = 0; j < n - 1; j++){
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            // apply file redirections on top of pipe connections
            apply_redirections(&p->commands[i]);

            //execute the command
            if (execvp(p->commands[i].args[0], p->commands[i].args) == -1){
                error_command_not_found(p->commands[i].args[0]);
                exit(EXIT_FAILURE);
            }
        }

        //Parent Process
        //After forking each child, the parent closes the pipe ends of each child
        //we do this inside the loop rather than after because we need to close them incrementally 
        //If the parent holds any write end open, the reading child will never see EOF and will hang 
        //The parent never reads or writes to the pipes itself so it should close every end as soon as possible
        if (i > 0){
            close(pipe_fds[i - 1][0]);
        }
        if (i < n - 1) {
            close(pipe_fds[i][1]);
        }
    }

    //wait for all children to finish before showing the prompt again
    for (int i = 0; i < n; i++){
        int status;
        waitpid(pids[i], &status, 0);
    }
}