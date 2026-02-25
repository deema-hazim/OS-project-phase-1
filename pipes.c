#include "pipes.h"
#include "redirects.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

//Handles execution of a pipeline of n commands connected by pipes
//p is a pointer to a pipeline struct containing all commands and their metadata
void execute_pipes(Pipeline *p) {
    int n = p->num_commands;

    /*
     * Allocate (n-1) pipes. Each pipe is an int[2] where:
     *   pipe_fds[i][0] = read end of pipe i
     *   pipe_fds[i][1] = write end of pipe i
     *
     * pipe i connects command i (writes) to command i+1 (reads)
     */
    int pipe_fds[MAX_PIPES - 1][2];

    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipe_fds[i]) == -1) {
            /* pipe() failed - OS couldn't create the pipe */
            perror("pipe");
            return;
        }
    }

    /* Store all child PIDs so we can wait for all of them at the end */
    pid_t pids[MAX_PIPES];

    for (int i = 0; i < n; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            /* fork() failed - OS couldn't create a new process */
            perror("fork");
            return;
        }

        if (pids[i] == 0) {
            /*
             * ── CHILD PROCESS ──
             *
             * Each child needs to:
             *   1. Connect its STDIN to the read end of the previous pipe (if not first command)
             *   2. Connect its STDOUT to the write end of the next pipe (if not last command)
             *   3. Close ALL pipe ends it doesn't use (critical - prevents hanging)
             *   4. Apply any file redirections (these override pipe connections if both exist)
             *   5. Execute its command via execvp()
             */

            /* Step 1: Connect STDIN to previous pipe's read end */
            if (i > 0) {
                /*
                 * dup2(old_fd, new_fd) makes new_fd point to the same place as old_fd.
                 * Here we replace STDIN (fd 0) with the read end of the previous pipe.
                 * After this, when the command reads from stdin, it reads from the pipe.
                 */
                if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
            }

            /* Step 2: Connect STDOUT to next pipe's write end */
            if (i < n - 1) {
                /*
                 * Replace STDOUT (fd 1) with the write end of the next pipe.
                 * After this, when the command writes to stdout, it goes into the pipe.
                 */
                if (dup2(pipe_fds[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
            }

            /*
             * Step 3: Close ALL pipe ends in the child.
             *
             * This is critical. Even after dup2(), the original pipe fds are still open.
             * If we don't close them:
             *   - The write end being open in a reader means it never sees EOF
             *   - Commands will hang indefinitely waiting for input that never ends
             *
             * We close every single pipe end - the child only needs STDIN/STDOUT now.
             */
            for (int j = 0; j < n - 1; j++) {
                close(pipe_fds[j][0]);
                close(pipe_fds[j][1]);
            }

            /*
             * Step 4: Apply file redirections if any exist on this command.
             *
             * This calls your partner's function. If command i has, say, "> out.txt",
             * apply_redirections() will dup2() STDOUT to that file, overwriting the
             * pipe connection we just set up. This is exactly what we want for
             * composed commands like: cmd1 | cmd2 > out.txt
             */
            apply_redirections(&p->commands[i]);

            /*
             * Step 5: Execute the command.
             *
             * execvp() takes the program name and argument array.
             * It searches PATH automatically (that's what the 'p' in execvp means).
             * If it returns at all, something went wrong.
             */
            if (execvp(p->commands[i].args[0], p->commands[i].args) == -1) {
                /*
                 * execvp failed. Most common reason: command not found.
                 * We print a specific error and exit the child process.
                 * The parent will still be waiting - it will see this child exit with failure.
                 */
                fprintf(stderr, "%s: command not found\n", p->commands[i].args[0]);
                exit(EXIT_FAILURE);
            }
        }

        /*
         * ── PARENT PROCESS ──
         *
         * After forking each child, the parent must close the pipe ends it no longer needs.
         *
         * Why do this inside the loop (not after)?
         * Because we fork one child at a time. Once child i is forked, pipe i-1's
         * read end and pipe i's write end are only needed by child i. The parent
         * holding them open would prevent EOF from being delivered to downstream commands.
         *
         * Rule of thumb: close a pipe end in the parent as soon as no future child needs it.
         */
        if (i > 0) {
            /* Close the read end of the previous pipe - child i was the last to need it */
            close(pipe_fds[i - 1][0]);
        }
        if (i < n - 1) {
            /* Close the write end of the next pipe - child i was the last to need it */
            close(pipe_fds[i][1]);
        }
    }

    /*
     * Wait for ALL child processes to finish before returning to the shell prompt.
     *
     * The spec says: "Your shell has to wait for the last process in the pipeline
     * to terminate to show the prompt for the next command."
     *
     * We wait for all children (not just the last) to avoid zombie processes.
     * waitpid() with the stored PID ensures we wait for each specific child.
     */
    for (int i = 0; i < n; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
}