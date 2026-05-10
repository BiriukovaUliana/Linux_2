#include <stdio.h>

void my_function() {
    int local_array[1000];     
    int local_init[1000] = {1};   
    local_array[0] = 42;
    printf("local_array[0] = %d\n", local_array[0]);
    printf("local_init[0]  = %d\n", local_init[0]);
}

int main() {
    printf("Hello, World!\n");
    my_function();
    return 0;
}
