// errors.c
// prints error messages to stderr

#include "myshell.h"

//PHASE 1 ERRORS

void error_missing_input_file(){
    fprintf(stderr, "Input file not specified.\n");
}

void error_missing_output_file(){
    fprintf(stderr, "Output file not specified.\n");
}

void error_missing_error_file(){
    fprintf(stderr, "Error output file not specified.\n");
}

void error_missing_command_after_pipe(){
    fprintf(stderr, "Command missing after pipe.\n");
}

void error_empty_command_between_pipes(){
    fprintf(stderr, "Empty command between pipes.\n");
}

void error_command_not_found(char *cmd){
    fprintf(stderr, "Command not found: %s\n", cmd);
}

void error_file_not_found(char *file){
    fprintf(stderr, "%s: file not found\n", file);
}


//PHASE 2 ERRORS

void error_socket_creation(){
    fprintf(stderr, "Failed to create socket.\n");
}

void error_connection_failed(){
    fprintf(stderr, "Error: unable to establish a connection to the remote socket.\n");
}

void error_server_disconnected(){
    fprintf(stderr, "Server disconnected.\n");
}

void error_bind_failed(){
    fprintf(stderr, "Failed to bind socket to address.\n");
}

void error_listen_failed(){
    fprintf(stderr, "Failed to listen on socket.\n");
}

void error_accept_failed(){
    fprintf(stderr, "Failed to accept client connection.\n");
}

void error_malloc_failed(){
    fprintf(stderr, "Memory allocation failed.\n");
}

void error_execution_failed(){
    fprintf(stderr, "Server error: failed to execute command.\n");
}


// phase 3

void error_thread_creation(){
    fprintf(stderr, "Failed to create thread for client.\n");
}