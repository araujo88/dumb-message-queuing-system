#include "../include/helpers.h"
#include "../include/constants.h"
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <time.h>

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

void debug(const char *file, int line, const char *format, ...)
{
    time_t now = time(NULL);
    char formatted_time[20];
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("[%s] [%s:%d] ", formatted_time, file, line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

int strncmp_s(const char *s1, const char *s2, size_t n)
{
    if (s1 == NULL || s2 == NULL)
    {
        fprintf(stderr, "Null pointer supplied to string comparison.\n");
        errno = EINVAL; // Set errno to indicate invalid argument
        return -1;      // Use -1 consistently to indicate errors
    }

    if (memchr(s1, '\0', BUFFER_SIZE) == NULL || memchr(s2, '\0', BUFFER_SIZE) == NULL)
    {
        fprintf(stderr, "String not null terminated within buffer size limit.\n");
        errno = EINVAL; // Invalid input as strings are not properly terminated
        return -1;
    }

    return strncmp(s1, s2, n); // Compare up to n characters only
}
