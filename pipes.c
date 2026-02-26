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

    for (int i = 0; i < n; i++){
        pids[i] = fork();

        if (pids[i] == -1){
            perror("fork");
            return;
        }

        if (pids[i] == 0){
            // CHILD PROCESS

            //connect stdin to read end of previous pipe
            // skip for first command because it reads from the terminal
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

            // step 3: close ALL pipe fds in the child
            // even though dup2 wired up stdin/stdout, the original
            // pipe fds are still open. if we don't close them the
            // reader never sees EOF and hangs forever
            for (int j = 0; j < n - 1; j++){
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            // step 4: apply file redirections on top of pipe connections
            // e.g. if this command has > out.txt, this overwrites the
            // pipe stdout connection we set in step 2 - which is correct
            apply_redirections(&p->commands[i]);

            // step 5: execute the command
            if (execvp(p->commands[i].args[0], p->commands[i].args) == -1){
                error_command_not_found(p->commands[i].args[0]);
                exit(EXIT_FAILURE);
            }
        }

        // ── PARENT PROCESS ──
        // close pipe ends the parent no longer needs after forking each child
        // must do this inside the loop to avoid blocking downstream readers
        if (i > 0){
            close(pipe_fds[i - 1][0]);
        }
        if (i < n - 1) {
            close(pipe_fds[i][1]);
        }
    }

    // wait for ALL children to finish before showing the prompt again
    for (int i = 0; i < n; i++){
        int status;
        waitpid(pids[i], &status, 0);
    }
}