/*
 * Dynamic Array implementation extracted and adapted from nob.h
 * Original author: Tsoding[](https://github.com/tsoding/nob.h)
 *
 * Original nob.h is dedicated to the public domain (Unlicense):
 * https://github.com/tsoding/nob.h/blob/master/nob.h
 *
 * This is a modified version split into separate headers for personal use.
 * No restrictions apply — you can use, modify, and distribute freely.
 */

#ifndef STRING_BUILDER_H_
#define STRING_BUILDER_H_

#include <stdarg.h>   // for va_list
#include <stdio.h>    // for vsnprintf
#include <string.h>   // for strlen, memcpy
#include "dynamicArray.h"

#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement PRINTF_FORMAT for MSVC
#    define PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typeDA(char, StringBuilder_St);

int sb_appendf(StringBuilder_St* sb, const char *fmt, ...) PRINTF_FORMAT(2, 3);
// Pads the StringBuilder_St (sb) to the desired word size boundary with 0s.
// Imagine we have sb that contains 5 `a`-s:
//
//   aaaa|a
//
// If we pad align it by size 4 it will look like this:
//
//   aaaa|a000| <- padded with 0s to the next size 4 boundary
//
// Useful when you are building some sort of binary format using StringBuilder_St.
void sb_padAlign(StringBuilder_St* sb, size_t size);

// Append a sized buffer to a string builder
#define sb_appendBuf(sb, buf, size) da_append_many(sb, buf, size)

// Append a NULL-terminated string to a string builder
#define sb_appendCStr(sb, cstr)  \
    do {                              \
        const char *s = (cstr);       \
        size_t n = strlen(s);         \
        da_append_many(sb, s, n); \
    } while (0)

// Append a single NULL character at the end of a string builder. So then you can
// use it a NULL-terminated C string
#define sb_appendNull(sb) da_append_many(sb, "", 1)

// Free the memory allocated by a string builder
#define sb_free(sb) FREE((sb).items)

#endif // STRING_BUILDER_H_

#ifdef STRING_BUILDER_IMPLEMENTATION

int sb_appendf(StringBuilder_St *sb, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    // NOTE: the new_capacity needs to be +1 because of the null terminator.
    // However, further below we increase sb->count by n, not n + 1.
    // This is because we don't want the sb to include the null terminator. The user can always sbAppendNull() if they want it
    da_reserve(sb, sb->count + n + 1);
    char *dest = sb->items + sb->count;
    va_start(args, fmt);
    vsnprintf(dest, n+1, fmt, args);
    va_end(args);

    sb->count += n;

    return n;
}

void sb_padAlign(StringBuilder_St *sb, size_t size) {
    size_t rem = sb->count%size;
    if (rem == 0) return;
    for (size_t i = 0; i < size - rem; ++i) {
        da_append(sb, 0);
    }
}

#endif // STRING_BUILDER_IMPLEMENTATION