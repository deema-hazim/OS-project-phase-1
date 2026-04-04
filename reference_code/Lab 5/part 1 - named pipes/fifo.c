/*
NAMED PIPES::
-------------

- A named pipe (also known as a FIFO) is one of the methods for inter-process communication.

- It is an extension to the traditional pipe concept on Unix.

- A traditional pipe is “unnamed” and lasts only as long as the process exists.

- A named pipe, however, can last as long as the system is up, beyond the life of
  the process. It can be deleted if no longer used.

- Usually a named pipe appears as a file and generally processes attach to it for
  inter-process communication. 
  (if you do "mkfifo fifo1" in terminal, then "ls -la", you will see it listed
  with p before the permissions. This p means that the typo of this file is a PIPE)

  A FIFO file is a special kind of file on the local storage which allows two or more
  processes to communicate with each other by reading/writing to/from this file.
*/
//__________________________________________________________________________________
/*
- A FIFO special file is entered into the filesystem by calling mkfifo() in C.

  Once we have created a FIFO special file in this way, any process can open it
  for reading or writing, in the same way as an ordinary file.

  If a process reads data from a pipe, the data is removed. 
  So a pipe is a communication channel rather than a permanent data repository.
*/
//__________________________________________________________________________________
/*
- Both reader & writer processes need to open the file simultaneously, 
  otherwise opening of file for read/write operations will be blocked. 

  It has to be open at BOTH ENDS simultaneously before you can proceed
  to do any input or output operations on it.

  Opening a FIFO for reading normally blocks until some other process opens
  the same FIFO for writing, and vice versa
*/
//_________________________________________________________________________________
/*
 Unlike regular files, no data is written in the disk when passing data
 between reader and writer.

 The kernel internally pipes the data between reader and writer processes.

 The FIFO special file has no contents on the file system; the file system entry
 merely serves as a reference point so that processes can access the pipe
 using a name in the file system.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SIZE 13

int main()
{

  //create a named pipe myfifo with read,write & execute permissions for all users
  //0777 is an octal representation for these permissions
  //this means that the named pipe "myfifo" can be read from or written to by ANYONE
  
  //mkfifo("myfifo1" , 0777);

  if (mkfifo("myfifo1" , 0777) == -1)
  {
    if (errno != EEXIST)
    {
      perror("Error: couldn't create myfifo pipe");
      exit(0);
    }
  }

  printf("Opening...\n");

  //open the pipe in WRITING MODE (because it wants to send some data)
  int fd = open("myfifo1" , O_WRONLY);

  printf("Opened\n");

  //use write system call to write the string "NAMED PIPES" to myfifo
  write(fd , "NAMED PIPES\n", SIZE);

  printf("Written\n");

  //close the fd 
  close(fd);

  printf("Closed\n");

  return 0;
 
}
/*
Special property for NAMED PIPES/FIFOS: 

It has to be open at BOTH ENDS simultaneously before you can proceed
to do any input or output operations on it.

open(): opening the read or write end of a FIFO BLOCKS until the other end is also 
opened by another process

So, if we open a fifo for WRITING, the open call BLOCKS until another process comes
along and opens the same fifo for READING. Then, the process that opened for writing
will continue

*/
/*
When you compile and run this code, it will open the pipe and display "Opening..."
then hang. It is now open for WRITING at one end

Now, open another terminal window, and write "cat myfifo" to READ the content of the 
pipe (so that is open for READING at the other end), then the block is removed
and code continues execution till the end.
*/