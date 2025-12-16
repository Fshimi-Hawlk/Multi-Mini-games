#ifndef STACKTRACE_H
#define STACKTRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define MAX_FRAMES    64
#define PATH_MAX_LENGTH  256

/**
 * Initialize the symbol handler. Call once at program start.
 */
int init_symbol_handler(void);

/**
 * Cleanup the symbol handler. Call once at program end.
 * (No-op on Linux, provided for API symmetry with Windows version.)
 */
void cleanup_symbol_handler(void);

/**
 * Fill `out` (of size `outSize`) with the caller info at
 * backtrace index `depth`.  E.g.:
 *   depth = 1 → immediate caller of the function that calls this.
 *
 * Returns 0 on success (even if symbol not found), <0 on error.
 */
int get_caller_info(char *out, size_t outSize, unsigned int depth);

/**
 * Convenience: print caller info at given `depth` (see above).
 */
#define print_caller_info_at_depth(depth)               \
    do {                                               \
        char __buf[512];                               \
        if (get_caller_info(__buf, sizeof __buf, depth) == 0) \
            puts(__buf);                               \
        else                                           \
            puts("Error getting caller info");         \
    } while(0)


#define print_caller_info() print_caller_info_at_depth(2)

#endif