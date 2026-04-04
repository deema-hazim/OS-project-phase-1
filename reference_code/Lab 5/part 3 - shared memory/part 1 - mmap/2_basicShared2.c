#include<sys/types.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/mman.h>

#define SIZE 4096


/*
void * mmap (void *address, 
            size_t length, 
            int protect,
            int flags,
            int filedes,
            off_t offset)


1- address:: This argument gives a preferred starting address for the mapping. 
If this argument is NULL, then the kernel can place the mapping anywhere it sees fit.

2- length:: This is the number of bytes which to be mapped.

3- protect:: This argument is used to control what kind of access is permitted. 
This argument may be logical ‘OR’ of the flags PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE. 
The access types of read, write and execute are the permissions on the content.

4- flags:: This argument is used to control the nature of the map. 
Some common values of the flags:
    -> MAP_SHARED: Multiple processes can share this mapping. 
    			   Changes made by one process are visible to others.
    
    ->MAP_ANONYMOUS: This flag is used to create an anonymous mapping.
                     Anonymous mapping means the mapping is not connected to any files.
                     (the fd and offset arguments are ignored if used). 

5- filedes:: This is the file descriptor which has to be mapped. (usually -1 if MAP_ANONYMOUS used)

6- offset:: This is offset from where the file mapping started. (usually 0 if MAP_ANONYMOUS used)


On success, the mmap() returns a pointer to the mapped area. 
On failure, the function returns MAP_FAILED.

___________________________________________________________________________________

To unmap the mapped region , munmap() function is used :


int munmap(void *address, size_t length);

Return values:
On success, the munmap() returns 0
On failure, the function returns -1.

*/




int v = 5;

int main(int argc, char** argv)
{

	//whenever the process is forked, this block of memory will be shared between the parent and the child
	int *shared_memory = mmap(NULL, SIZE, 
							PROT_READ | PROT_WRITE,
							MAP_SHARED | MAP_ANONYMOUS, -1,0);

	
	*shared_memory = 20;

    //fork() after mmap() : the child inherits that same mapping.
	
	if (fork() == 0) 
	{
		//IN CHILD
		*shared_memory = 30; //changing valiable value
		
	}
	else
	{
		//IN PARENT
		wait(NULL); 
		printf("V == %d\n", *shared_memory);
	}

}