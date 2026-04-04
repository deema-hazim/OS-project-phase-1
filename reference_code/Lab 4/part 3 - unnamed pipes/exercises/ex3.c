#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
  int fd[2];
  char buff[100];

  pipe(fd);

  pid_t childId = fork();

  if (childId<0)
  {
      perror("Fork failed");
      exit(1);
  }
  else
  {
    //CHILD
    if (childId == 0) {
      int nChars = 0;

    //reading so close the write end of pipe
    close(fd[1]);

    // Receive characters from parent process via pipe
    // one at a time, and count them
    char c;
    while (read(fd[0], &c, 1) == 1 )
      nChars+=1;
      
    printf("Child Read = %d\n", nChars);
    return nChars;
  }

  //PARENT
  else 
  {
    int nChars = 0;
    int size = 0;

    //will write to pipe so close the read end 
    close(fd[0]);

    // Send characters from command line arguments starting with
    // argv[1] one at a time through pipe to child process.
    
    for (int i = 1; i < argc; i++) {
      size = strlen(argv[i]);
      for (int z = 0; z < size; z++) 
        write(fd[1], &argv[i][z], 1);      
    }

    //done with writing so close write end of pipe
    close(fd[1]);

    /* Wait for child process to return and
       Receive number of characters counted via the parent */
    int status;
    wait(&status);

    int return_value = WEXITSTATUS(status);
    printf("Child count received %d chars\n", return_value);
    return 0;
  }

  //close both ends
  close(fd[0]);
  close(fd[1]);
}
  return 0;
}
