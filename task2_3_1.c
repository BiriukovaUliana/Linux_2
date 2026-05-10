#include <stdio.h>
#include <stdlib.h>

int   data_int   = 42;
float data_float = 3.14;

int  bss_int;
char bss_array[100];

int main() {
    int  stack_var = 10;                  
    int *heap_ptr  = malloc(sizeof(int));   
    *heap_ptr = 99;

    printf("text  (main):       %p\n", (void*)main);
    printf("data  (data_int):   %p\n", &data_int);
    printf("data  (data_float): %p\n", &data_float);
    printf("bss   (bss_int):    %p\n", &bss_int);
    printf("bss   (bss_array):  %p\n", &bss_array);
    printf("heap  (heap_ptr):   %p\n", heap_ptr);
    printf("stack (stack_var):  %p\n", &stack_var);

    free(heap_ptr);
    return 0;
}
