// This program opens the same file ("shared.txt"),
// maps it into memory as read-only, and prints whatever was written into it.
#include <stdio.h>      
#include <stdlib.h>     
#include <fcntl.h>      
#include <sys/mman.h>   
#include <unistd.h>     
#include <sys/stat.h>   

#define SIZE 4096       


int main(void) {
    const char *path = "./shared.txt";   

    //Open the file for reading
    int fd = open(path, O_RDONLY);
    if (fd == -1) { perror("open"); exit(1); }

    //   Map the file into memory.
    //    - PROT_READ means we only need to read, not write.
    //    - MAP_SHARED means we see changes made by other processes.
    char *shared = mmap(NULL, SIZE,
                        PROT_READ,
                        MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) { perror("mmap"); exit(1); }

    close(fd); 

    //Print whatever text is in the mapping.
    printf("[reader] read message: %s", shared);

    //Unmap the region when done.
    munmap(shared, SIZE);
    return 0;
}
