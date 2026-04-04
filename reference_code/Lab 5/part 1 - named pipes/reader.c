// C program to implement one side of FIFO 
// This side reads first, then writes 

//run writer and reader files in two separate terminal windows

#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#define LENGTH 200

int main() 
{ 
    int fd1; 
  
    // FIFO file path 
    char * myfifo = "myfifo"; 
  
    mkfifo(myfifo, 0777); 
  
    char str1[LENGTH], str2[LENGTH]; 

    while (1) 
    { 
        // First open in read only and read 
        fd1 = open(myfifo,O_RDONLY); 
        read(fd1, str1, LENGTH); 
  
        // Print the read string and close 
        printf("User 1: %s\n", str1); 
        close(fd1); 
  
        // Now open in write mode and write 
        // string taken from user. 
        fd1 = open(myfifo,O_WRONLY); 
        fgets(str2, LENGTH, stdin); 
        write(fd1, str2, strlen(str2)+1);    //+1 to account for \0 at the end of strings in C
        close(fd1); 
    } 
    return 0; 
} 