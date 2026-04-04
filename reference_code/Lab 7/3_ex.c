#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

#define NUM_THREADS 4

int array[] = {20, 18, 16, 14, 12, 10, 8, 6, 4, 2, -10, -20, -30, -40, 15, 23, 25, 75, 45, 33};

// Function that calculates sum of 5 elements of an array
void *ArraySum(void *arg)
{
    int id = (int)arg;
    int partial_sum = 0;
    int begin = id * (NUM_THREADS + 1);
    int end = id * (NUM_THREADS + 1) + NUM_THREADS;

    // Iterating from index low to high
    for(int i = begin; i <= end; i++)
    {
        // Accumulating array partial sum
        partial_sum += array[i];
    }
    // Exiting current thread
    pthread_exit((void *)partial_sum);
}
// Main function
int main(int argc, char *argv[])
{
    int rc, t;
    int arraySum = 0;
    void *retvalue;

    // Creating thread array
    pthread_t threads[NUM_THREADS];

    // Iterating over each thread
    for(t = 0; t < NUM_THREADS; t++)
    {
        // Creating a thread
        rc = pthread_create(&threads[t], NULL, ArraySum, (void *)t);
        
    }
    /* Waiting till all threads finish their execution */
    for (t = 0; t < NUM_THREADS; t++)
    {
         pthread_join(threads[t], &retvalue);
         arraySum += (int)retvalue;
    }
    // Printing Array sum
    printf("\nArray Sum: %d \n", arraySum);
    pthread_exit(NULL);
}



/*
int pthread_join(pthread_t thread, void **retval);

The pthread_join() function waits for the thread specified by
thread to terminate.  If that thread has already terminated, then
pthread_join() returns immediately. 

If retval is not NULL, then pthread_join() copies the exit status
of the target thread (i.e., the value that the target thread
supplied to pthread_exit()) into the location pointed to by
retval.  

*/