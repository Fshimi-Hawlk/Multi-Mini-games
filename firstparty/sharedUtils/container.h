/**
    @file firstparty/utils/container.h
    @author Fshimi-Hawlk
    @date 2026-04-03
    @date 2026-04-03
    @brief Container and array utilities (print macros, defer, cast helpers, etc.).
*/

#ifndef FIRSTPARTY_UTILS_CONTAINER_H
#define FIRSTPARTY_UTILS_CONTAINER_H

#include "sharedUtils/debug.h"

#include <assert.h>

/**
    @brief Casts a variable to a new type using its address.
*/
#define castTo(newType) *(newType*)

/**
    @brief Convert a boolean value to string.
*/
#define boolStr(v) (v) ? "true" : "false"

/**
    @brief Returns the number of elements in a **static** array.
*/
#define ARRAY_LEN(array) (sizeof(array)/ (f32) sizeof(*array))

/**
    @brief Safely accesses an array element with bounds assertion.
*/
#define ARRAY_GET(array, index) \
    (assert((size_t)index < ARRAY_LEN(array)), array[(size_t)index])

/**
    @brief Defers return in a block: sets result and jumps to defer label.

    Useful for cleanup code at the end of a function.
*/
#define return_defer(value) do { result = (value); goto defer; } while(0)

/**
    @brief Prints all items in an array using a printf format.

    Appends a newline after.
*/
#define array_printContent(typeFmt, a, count) \
do { \
    for (u64 i = 0; i < (count); ++i) { \
        printf(typeFmt, a[i]); \
    } \
    nl \
} while (0)

/**
    @brief Prints all items in an array using a custom printf.

    Appends a newline after.
*/
#define array_printContentCustom(typePrintFn, a, count) \
do { \
    for (u64 i = 0; i < (count); ++i) { \
        typePrintFn(a[i]); \
    } \
    nl \
} while (0)

/**
    @brief Prints all items in a dynamic array using a printf format.

    Appends a newline after.
*/
#define da_printContent(typeFmt, da) array_printContent((typeFmt), (da)->items, (da)->count)

/**
    @brief Prints all items in a dynamic array using a custom printf.

    Appends a newline after.
*/
#define da_printContentCustom(typePrintFn, da) array_printContentCustom((typePrintFn), (da)->items, (da)->count)

#endif // FIRSTPARTY_UTILS_CONTAINER_H