#include "../include/helpers.h"
#include <assert.h>
#include <stdarg.h>

int randint(int min_num, int max_num)
{
    assert(min_num <= max_num);
    int range = max_num - min_num + 1;
    return rand() % range + min_num;
}

void clean_string(char *str, char c)
{
    char *pr = str, *pw = str;
    while (*pr)
    {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

void debug(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}
