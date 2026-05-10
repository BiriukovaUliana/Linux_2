#include <stdio.h>

void deep_function() {
    int  big1[1000];
    int  big2[1000];
    char big3[2000];

    big1[0] = 1; big2[0] = 2; big3[0] = 3;

    int top;
    printf("  Після виклику deep_function():\n");
    printf("  вершина стека (top): %p\n", &top);
    printf("  big1[0]:             %p\n", &big1[0]);
    printf("  big2[0]:             %p\n", &big2[0]);
    printf("  big3[0]:             %p\n", (void*)&big3[0]);
}

int main() {
    int i;
    printf("  До виклику функції:\n");
    printf("  вершина стека (i):   %p\n\n", &i);
    deep_function();
    return 0;
}
