/*Pipes connect processes.
 Anything can be written at the write end, 
 and read from the read end in the order it came in. 
 Pipe is unidirectional.

In this example, you can see how to create, write to, 
and read from a pipe.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 8

/*pipe() takes an array of two ints as an argument. 
    pipe() fills in that array with two file descriptors. 
    The first element of the array is the reading-end of the pipe pipefd[0], 
    the second is the writing end pipefd[1].

    When any bytes are written to pipefds[1],
    the operating system makes them available for reading 
    from pipefds[0]

    On success, pipe() will return 0, otherwise return -1

*/

int main()
{
    int pipefds[2];  //for the 2 file descriptions for two ends of the pipe
    char buf[30];

    //create pipe
    if (pipe(pipefds) == -1) 
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    //write to pipe (passing data "OS Pipes" down the write end of the pipe)
    printf("writing to file descriptor #%d\n", pipefds[1]);
    write(pipefds[1], "OS Pipes", SIZE);

    //read from pipe (read from reading end of the pipe)
    printf("reading from file descriptor #%d\n", pipefds[0]);
    read(pipefds[0], buf, SIZE);
    printf("read \"%s\"\n", buf);


    return 0;
}