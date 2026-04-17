#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


//constants
#define MAX_ARGS 64 // max arguments per command
#define MAX_PIPES 16 // maximum commands in a pipeline
#define MAX_INPUT 1024 // maximum length of one input line

// FOR PHASE 2 (CLIENT-SERVER COMMUNICATION)
#define PORT 12345 // TCP port the server listens
#define BUFFER_SIZE 4096 // buffer size for sending or receiving data over the socket


// structs
//Command Represents a single command with its arguments and any redirections
typedef struct {
    char *args[MAX_ARGS];
    char *input_file;
    char *output_file;
    char *error_file;
} Command;

//Pipeline represents a full input line, potentially with multiple commands connected by pipes
typedef struct {
    Command commands[MAX_PIPES];
    int num_commands;
} Pipeline;

//parser functions (parser.c)
// Initializes a pipeline to a clean empty state and is called before parse_input()
void init_pipeline(Pipeline *p);

// Parses raw input string into a Pipeline struct
// returns 0 on success, -1 on error
int parse_input(char *input, Pipeline *p);

/* EXECUTOR FUNCTIONS (executor.c) */

/* Main dispatch function - decides whether to run single command or pipeline */
void execute_pipeline(Pipeline *p);

/* Executes a single command with no pipes */
void execute_single(Command *cmd);


//Redirection functions (redirects.c)

/* Applies input/output/error redirections for a command using dup2() */
void apply_redirections(Command *cmd);


//Pipe Functions (pipes.c)
// Executes n commands connected by pipes
void execute_pipes(Pipeline *p);


//error functions (errors.c)
//PHASE 1 ERRORS
void error_missing_input_file();
void error_missing_output_file();
void error_missing_error_file();
void error_missing_command_after_pipe();
void error_empty_command_between_pipes();
void error_command_not_found(char *cmd);
void error_file_not_found(char *file);

//PHASE 2 ERRORS
void error_socket_creation();
void error_connection_failed();
void error_server_disconnected();
void error_bind_failed();
void error_listen_failed();
void error_accept_failed();
void error_malloc_failed();
void error_execution_failed();

//phase 3
void error_thread_creation();

#endif
