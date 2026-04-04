/*
Issue: sizeof(name) includes the null terminator, so it returns 5 instead of 4.
Fix: Use strlen to count characters excluding the null terminator.
*/

#include <stdio.h>
#include <string.h>

int main(void) {
    char name[] = "John";
    printf("%zu\n", strlen(name));
    return 0;
}