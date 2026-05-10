#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>

int main() {
    printf("Інформація про time_t\n");
    printf("Розмір time_t: %zu байт (%zu біт)\n",
           sizeof(time_t), sizeof(time_t) * 8);
    printf("Розмір int:    %zu байт (%zu біт)\n",
           sizeof(int), sizeof(int) * 8);
    printf("Розмір long:   %zu байт (%zu біт)\n",
           sizeof(long), sizeof(long) * 8);

    time_t max_time;

    if (sizeof(time_t) == 4) {
        max_time = (time_t)INT32_MAX;
        printf("\nАрхітектура: 32-бітний time_t\n");
    } else {
        max_time = (time_t)INT64_MAX;
        printf("\nАрхітектура: 64-бітний time_t\n");
    }

    printf("\nКоли time_t закінчиться?\n");
    printf("Максимум: %ld\n", (long)max_time);

    struct tm *overflow_tm = gmtime(&max_time);
    if (overflow_tm != NULL) {
        printf("Переповнення: %04d-%02d-%02d %02d:%02d:%02d UTC\n",
               overflow_tm->tm_year + 1900,
               overflow_tm->tm_mon + 1,
               overflow_tm->tm_mday,
               overflow_tm->tm_hour,
               overflow_tm->tm_min,
               overflow_tm->tm_sec);
    } else {
        printf("Для 64-біт: переповнення ~рік 292277026596\n");
    }

    time_t now = time(NULL);
    printf("\nПоточний час (секунди від 1970): %ld\n", (long)now);

    printf("\nДемонстрація переповнення\n");
    int32_t t32_max = INT32_MAX;
    int32_t t32_over = t32_max + 1;
    printf("INT32_MAX     = %d\n", t32_max);
    printf("INT32_MAX + 1 = %d  (від'ємне!)\n", t32_over);

    return 0;
}
