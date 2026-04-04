/*
Issue: Used '->' on a non-pointer struct variable.
Fix: Use '.' for direct struct variables, '->' only for pointers.
*/

#include <stdio.h>

struct Book {
    char title[64];
    int pages;
};

int main(void) {
    struct Book b = {"OS Concepts", 500};
    printf("%s has %d pages\n", b.title, b.pages);

    struct Book *bp = &b;
    printf("%s has %d pages (via pointer)\n", bp->title, bp->pages);
    return 0;
}