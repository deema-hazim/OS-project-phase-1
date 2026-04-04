/*
Issue: The pointer 'p' was uninitialized and dereferenced, causing undefined behavior.
Fix: Initialize the pointer to a valid memory location before dereferencing.
*/

#include <stdio.h>

int main(void) {
    int x = 0;
    int *p = &x;   // or int *p = malloc(sizeof(int));
    *p = 10;
    printf("%d\n", *p);
    return 0;
}