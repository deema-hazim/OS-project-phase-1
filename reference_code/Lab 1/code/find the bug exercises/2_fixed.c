/*
Issue: Attempted to modify a string literal, which is read-only.
Fix: Use a character array instead, which is modifiable.
*/

#include <stdio.h>

int main(void) {
    char s[] = "Hello";
    s[0] = 'Y';
    printf("%s\n", s);
    return 0;
}