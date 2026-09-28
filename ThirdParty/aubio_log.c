#include <stdarg.h>
#include <stdio.h>

typedef unsigned int uint_t;
typedef int sint_t;
typedef char char_t;

uint_t aubio_log (sint_t level, const char_t *fmt, ...)
{
    (void) level;

    va_list args;
    va_start (args, fmt);
    vfprintf (stderr, fmt, args);
    va_end (args);

    return 0;
}