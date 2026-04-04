/*
Issue: In C, struct names must be prefixed with 'struct' unless typedef is used.
Fix: Add a typedef for convenience or use 'struct Employee' explicitly.
*/

#include <stdio.h>

typedef struct Employee {
    int id;
    float salary;
} Employee;

int main(void) {
    Employee e = {101, 5000.0};  //option 2: add struct before Employee e without the earlier typedef
    printf("ID: %d, Salary: %.2f\n", e.id, e.salary);
    return 0;
}