#include <stdio.h>

struct Book {
    char title[64];
    int pages;
};

int main(void) {
    struct Book b = {"OS Concepts", 500};
    printf("%s has %d pages\n", b->title, b->pages);
    return 0;
}