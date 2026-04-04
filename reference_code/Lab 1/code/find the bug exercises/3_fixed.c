/*
Issue: Accessing argv[1] without checking if argc >= 2 can cause out-of-bounds access.
Fix: Add a check on argc before using argv[1].
*/

#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No argument provided!\n");
        return 1;
    }
    printf("First arg: %s\n", argv[1]);
    return 0;
}