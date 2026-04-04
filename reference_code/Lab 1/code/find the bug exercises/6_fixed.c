/*
Issue: a / b performs integer division before converting to float.
Fix: Cast to float before division to get fractional result.
*/

#include <stdio.h>

int main(void) {
    int a = 5, b = 2;
    float c = (float)a / b;
    printf("%f\n", c);
    return 0;
}