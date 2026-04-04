// Simulate a chat application
// C program to implement one side of FIFO 
// This side writes first, then reads 

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
    int fd; 
  
    // FIFO file path 
    char * myfifo = "myfifo"; 
  
    mkfifo(myfifo, 0777); 
  
    char arr1[LENGTH], arr2[LENGTH]; 

    while (1) 
    { 
        // Open FIFO for write only 
        fd = open(myfifo, O_WRONLY); 
  
        // Take an input from user. 
        // LENGTH 200 is maximum length 
        fgets(arr2, LENGTH, stdin); 
  
        // Write the input arr2 on FIFO 
        // and close it 
        write(fd, arr2, strlen(arr2)+1);  //+1 to account for \0 at the end of strings in C
        close(fd); 
  


        // Open FIFO for Read only 
        fd = open(myfifo, O_RDONLY); 
  
        // Read from FIFO 
        read(fd, arr1, sizeof(arr1)); 
  
        // Print the read message 
        printf("User 2: %s\n", arr1); 
        close(fd); 
    } 
    return 0; 
} 