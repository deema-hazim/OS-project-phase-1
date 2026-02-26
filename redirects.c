// redirects.c
// handles output and error redirection using dup2()
// similar to how the pipe examples in lab 4 use dup2 to redirect stdout/stdin

#include "myshell.h"

// if the command has > or 2> set, open the file and
// use dup2 to point stdout or stderr to that file instead
void apply_redirections(Command *cmd){

    // handle > (output redirection)
    // open the file for writing, create it if it doesnt exist, truncate if it does
    if(cmd->output_file != NULL){
        int fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd < 0){
            perror("open");
            exit(EXIT_FAILURE);
        }
        // replace stdout (fd 1) with our file
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    // handle 2> (error redirection)
    if(cmd->error_file != NULL){
        int fd = open(cmd->error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(fd < 0){
            perror("open");
            exit(EXIT_FAILURE);
        }
        // replace stderr (fd 2) with our file
        dup2(fd, STDERR_FILENO);
        close(fd);
    }

    // handle < (input redirection)
    if(cmd->input_file != NULL){
        int fd = open(cmd->input_file, O_RDONLY);
        if(fd < 0){
            error_file_not_found(cmd->input_file);
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
}
