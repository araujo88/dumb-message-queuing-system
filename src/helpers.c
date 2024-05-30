#include "../include/helpers.h"
#include "../include/constants.h"
#include <assert.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

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

int strncmp_s(const char *s1, const char *s2)
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

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    // Use the minimum of len1 and len2 to avoid reading beyond the end of the shorter string
    return strncmp(s1, s2, len1 < len2 ? len1 : len2);
}
