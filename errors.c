// errors.c - all the error messages for the shell
// each function prints a specific error to stderr

#include "myshell.h"

void error_missing_input_file()
{
    fprintf(stderr, "Input file not specified.\n");
}

void error_missing_output_file()
{
    fprintf(stderr, "Output file not specified.\n");
}

void error_missing_error_file()
{
    fprintf(stderr, "Error output file not specified.\n");
}

void error_missing_command_after_pipe()
{
    fprintf(stderr, "Command missing after pipe.\n");
}

void error_empty_command_between_pipes()
{
    fprintf(stderr, "Empty command between pipes.\n");
}

void error_command_not_found(char *cmd)
{
    fprintf(stderr, "%s: command not found\n", cmd);
}

void error_file_not_found(char *file)
{
    fprintf(stderr, "%s: file not found\n", file);
}
