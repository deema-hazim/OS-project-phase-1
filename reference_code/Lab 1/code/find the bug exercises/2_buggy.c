#include <stdio.h>

int main(void) {
    char *s = "Hello";
    s[0] = 'Y';
    printf("%s\n", s);
    return 0;
}