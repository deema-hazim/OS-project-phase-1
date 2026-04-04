#include <stdio.h>

int main(void) {
    FILE *fp = fopen("data.txt", "r");
    char buffer[100];
    fgets(buffer, 100, fp);
    printf("Read line: %s\n", buffer);
    fclose(fp);
    return 0;
}