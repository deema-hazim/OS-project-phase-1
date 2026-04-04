/*
Issue: fopen may return NULL, but the code does not check before using it.
Fix: Verify that fopen succeeded before reading from the file.
*/

#include <stdio.h>

int main(void) {
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        perror("Error opening data.txt");
        return 1;
    }
    char buffer[100];
    if (fgets(buffer, sizeof buffer, fp) != NULL) {
        printf("Read line: %s\n", buffer);
    }
    fclose(fp);
    return 0;
}