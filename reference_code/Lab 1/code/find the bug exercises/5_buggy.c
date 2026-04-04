#include <stdio.h>

int main(void) {
    char name[] = "John";
    printf("%lu\n", (unsigned long)sizeof(name));
    return 0;
}