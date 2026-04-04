#include <stdio.h>    // puts(), printf(), perror(), getchar()
#include <stdlib.h>   // exit(), EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>   // getpid(), getppid(),fork()
#include <sys/wait.h> // wait()

void child() {
  int status = 42; //any non zero value

  printf(" CHILD <%ld> with PPID = %ld is sleeping. Wait....\n",
         (long) getpid(), (long) getppid());

  sleep(3);

  printf(" CHILD <%ld> I am awake now, Goodbye! Exit with status %d.\n",
         (long) getpid(), status);
 
  exit(status);
}

void parent(pid_t pid) {
  int status;

  printf("PARENT <%ld> Spawned a child with PID = %ld.\n",
         (long) getpid(), (long) pid);

  //wait for a specific child process with specific pid
  //third argument is for options such as :
  //WNOHANG: return immediately if no child has exited
  //WUNTRACED: return if a child has stopped
  //0 is default for no options
  waitpid(pid , &status , 0);

  printf("PARENT <%ld> had a terminated child which had a PID = %ld.\n",
    (long) getpid(), (long) pid);


  if (WIFEXITED(status)) {
    printf("PARENT <%ld> Child with PID = %ld and exit status = %d terminated.\n",
           (long) getpid(), (long) pid, WEXITSTATUS(status));
  }

  printf("PARENT <%ld> Goodbye!\n",
         (long) getpid());

  exit(EXIT_SUCCESS);
}

int main(void) {

  pid_t pid;

  switch (pid = fork()) {
  case -1:
    // On error fork() returns -1.
    perror("fork failed");
    exit(EXIT_FAILURE);
  case 0:
    // On success fork() returns 0 in the child.
    child();
  default:
    // On success fork() returns the pid of the child to the parent.
    parent(pid);
  }
}


