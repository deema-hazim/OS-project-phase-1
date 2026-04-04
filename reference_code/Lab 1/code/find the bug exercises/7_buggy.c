#include <stdio.h>

struct Employee {
    int id;
    float salary;
};

int main(void) {
    Employee e = {101, 5000.0};
    printf("ID: %d, Salary: %.2f\n", e.id, e.salary);
    return 0;
}