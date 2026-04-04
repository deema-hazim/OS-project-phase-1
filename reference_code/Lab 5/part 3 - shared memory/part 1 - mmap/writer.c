//Now, what if we want two totally separate programs (not just forked children) to share memory?

// This program creates/opens a file called "shared.txt",
// resizes it to SIZE bytes, maps it into memory, and writes a string into it.
// Any other program that maps the same file with MAP_SHARED will see the changes.


//run first in one terminal, then run the reader in another terminal


#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <fcntl.h>      
#include <sys/mman.h>   
#include <unistd.h>     
#include <sys/stat.h>   

#define SIZE 4096       // size of the shared memory region 


int main(void) {
    const char *path = "./shared.txt";   // file path
   

    //    Open or create the file for read/write access.
    //    0600 permissions: read+write for owner only.
    //    If the file doesn't exist, O_CREAT creates it as an empty file.
    int fd = open(path, O_RDWR | O_CREAT, 0600);
    if (fd == -1) { perror("open"); exit(1); }

    //    Make sure the file is at least SIZE bytes long.
    //    Because mmap needs the file to have enough space.
    //    If the file is smaller than the mapping size, mmap() will fail.
    //    ftruncate enlarges (or shrinks) the file to exactly SIZE bytes.
    if (ftruncate(fd, SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Create a shared mapping of the file into memory.
    //    - NULL lets the kernel pick the address.
    //    - SIZE is how many bytes to map.
    //    - PROT_READ | PROT_WRITE means we can read/write the memory.
    //    - MAP_SHARED means changes go back to the file and are visible to other mappings.
    //    - fd is the file backing the mapping.
    //    - offset 0 means map from the beginning of the file.
    char *shared = mmap(NULL, SIZE,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
    
    if (shared == MAP_FAILED) { perror("mmap"); exit(1); }

    // Once mapped, the file descriptor is no longer needed for access.
    // We can close it; the mapping still exists.
    close(fd);

    // 5. Write some text into the mapped memory.
    const char *msg = "Hello via mmap and a .txt file!\n";
    strcpy(shared, msg);

    printf("[writer] wrote message: %s", msg);

    printf("[writer] press Enter to exit...");
    getchar();

    //unmap the region when finished.
    munmap(shared, SIZE);
    return 0;
}
