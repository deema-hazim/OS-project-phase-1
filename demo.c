// demo.c
// test program for the scheduler, prints one line per second for n seconds

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if(n < 0) n = 0;

    // n+1 prints with n sleeps in between, total run time is n seconds
    for(int i = 1; i <= n; i++){
        printf("Demo %d/%d\n", i, n);
        fflush(stdout); // pipes are buffered, flush so server sees each line
        if(i < n) sleep(1);
    }

    return 0;
}
