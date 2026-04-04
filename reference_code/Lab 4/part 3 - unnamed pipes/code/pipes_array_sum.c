/*

Calculate sum of numbers from 1 to 100

Divide a array of integers into two arrays

The child process calculates the sum of the left half

The parent process calculates the sum of the right half

After the calculation of the child process is completed, the result is written into
the pipe.

After the parent process waits for the calculation of the child process to complete,
it obtains the calculation result of the child process from the pipe. 

Finally, the parent process adds its calculation result to the calculation result
of the child process to get the sum of the entire array

*/


#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define N 101
#define READ_END 0
#define WRITE_END 1

int main() {
    pid_t pid;
    int fd[2];
    int arr[N], i;
    

    // Initialize the array
    for (i = 0; i < N; i++) {
        arr[i] = i;
    }

    // Create pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed.\n");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed.\n");
        return 1;
    }

    // CHILD process
    if (pid == 0) {
        
        close(fd[READ_END]); // Close the reading end of the pipe
        
        int ans = 0, i;
        
        for (i = 0; i < N / 2; i++) { //left half from 0 till n/2
            ans += arr[i];
        }
      
        write(fd[WRITE_END], &ans, sizeof(int)); //write sum (ans) to pipe
      
        close(fd[WRITE_END]);// Close the write end of the pipe

    } 

    // PARENT process
    else if (pid > 0) {
        
        close(fd[WRITE_END]); // Close the write end of the pipe
        int right = 0, i;

        for (int i = N / 2; i < N; i++) { //right half from n/2 till end
            right += arr[i];
        }
        
        wait(NULL); // The parent process waits for the child process to complete
        
        //statements executing after wait() will be executed AFTER child finishes executing
        //So now we know that CHILD already wrote its sum to the pipe
        
        int left;
        
        read(fd[READ_END], &left, sizeof(int)); //read left half sum from pipe
        
        close(fd[READ_END]);// Close the reading end of the pipe
        
        int sum = left + right;
       
        printf("The sum is %d\n", sum);
    }
    return 0;
}
