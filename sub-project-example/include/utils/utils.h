/**
 * @file utils.h
 * @author 
 * @date 
 * @brief General utility macros and helper functions.
 */

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

/**
 * @brief Invalid pointer value used as a sentinel.
 */
#define INVALID_PTR (void *) -1

/**
 * @brief Debug macro to log when a function is called (uses log_debug).
 */
#define dcall log_debug("called")

/**
 * @brief Returns the sign of a value: -1 if negative, 1 otherwise (0 becomes 1).
 */
#define signof(v) ((v) < 0 ? -1 : 1)

/**
 * @brief Standard min/max macros.
 */
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Converts a number of bytes to KiB/MiB/GiB.
 */
#define KiB(n) ((u64) (n) << 10)
#define MiB(n) ((u64) (n) << 20)
#define GiB(n) ((u64) (n) << 30)

/**
* @brief Aligns a value upward to the nearest multiple of a power-of-2.
 *
 * Useful for memory alignment in arenas or buffers.
 * Formula: (n + p - 1) & ~(p - 1)
 *
 * @note Assumes 'p' is a power of 2; if not, behavior is undefined.
 *
 * @param n Value to align.
 * @param p Power-of-2 alignment (must be power of 2).
 */
#define alignUpPow2(n, p) (((u64) (n) + (u64) (p) - 1) & (~((u64) (p) - 1)))

#define castTo(newType) *(newType*) &

/*

*/
/**
 * @brief Swaps two variables of the same type using XOR (no temporary needed).
 *
 * Xor proprety:
 * V(x), x ^ x = 0
 *
 * a ^= b; // a ^ b
 * b ^= a; // b ^ (a ^ b) = b ^ b ^ a = 0 ^ a = a
 * a ^= b; // (a ^ b) ^ a = b ^ a ^ a = b ^ 0 = b
 * Works only if a and b are not the same variable and are integer types.
 */
#define swap(a, b) do { a ^= b; b ^= a; a ^= b; } while (0)

/**
 * @brief Clamps a value between min and max.
 */
#define clamp(v, _min, _max) min(max((_min), (v)), (_max))

/**
 * @brief Constructs a Vector2 by adding/subtracting/multiplying/dividing components.
 */
#define vec2Add(v1, v2, T) (T) { .x = (v1).x + (v2).x, .y = (v1).y + (v2).y }
#define vec2Sub(v1, v2, T) (T) { .x = (v1).x - (v2).x, .y = (v1).y - (v2).y }
#define vec2Mul(v1, v2, T) (T) { .x = (v1).x * (v2).x, .y = (v1).y * (v2).y }
#define vec2Div(v1, v2, T) (T) { .x = (v1).x / (v2).x, .y = (v1).y / (v2).y }

/**
 * @brief Constructs a Vector2 by adding/subtracting a value to the components.
 */
#define vec2AddVal(v, val, T) (T) { .x = (v).x + (val), .y = (v).y + (val) }
#define vec2Scale(v, scalar, T) (T) { .x = (v).x * (scalar), .y = (v).y * (scalar) }

/**
 * @brief Vector2 helper to print the components.
 */
#define vec2Fmt(vec2) (vec2).x, (vec2).y
#define vec2fStr "%.2f, %.2f"
#define vec2siStr "%d, %d"
#define vec2uiStr "%u, %u"

/**
 * @brief Add a new line.
 */
#define nl printf(LINE_END);

/**
 * @brief Convert a boolean value to string.
 */
#define boolStr(v) (v) ? "true" : "false"

// from nob.h
#ifdef _WIN32
#    define LINE_END "\r\n"
#else
#    define LINE_END "\n"
#endif

/**
 * @brief Set a variable to an unused state making the compiler stop complaining while developing.
 */
#define UNUSED(value) (void)(value)

/**
 * @brief Create a stoping point with extra message for developement purpose.
 */
#define TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

/**
 * @brief Use whenever something should never happen, not from human mistake but rather from memory corruption or something of that style.
 */
#define UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

/**
 * @brief Returns the number of elements in a **static** array.
 */
#define ARRAY_LEN(array) (sizeof(array)/ (f32) sizeof(*array))

/**
 * @brief Safely accesses an array element with bounds assertion.
 */
#define ARRAY_GET(array, index) \
    (assert((size_t)index < ARRAY_LEN(array)), array[(size_t)index])

/**
 * @brief Defers return in a block: sets result and jumps to defer label.
 *
 * Useful for cleanup code at the end of a function.
 */
#define return_defer(value) do { result = (value); goto defer; } while(0)

// end from nob.h

/**
 * @brief Shuffles an array in-place using Fisher-Yates algorithm.
 *
 * Requires prng_rand() to be available.
 */
#define shuffleArray(array, size) \
do { \
    for (u32 i = size - 1; i > 0; --i) { \
        u32 r = prng_rand() % (i + 1); \
        if (r == i) continue; \
        swap(array[i], array[r]); \
    } \
} while (0)

/**
 * @brief Shuffles a dynamic array's items.
 */
#define da_shuffle(da) shuffleArray((da)->items, (da)->count)

/**
 * @brief Prints all items in a dynamic array using a printf format.
 *
 * Appends a newline after.
 */
#define da_printContent(typeFmt, da) \
do { \
    for (u64 i = 0; i < (da)->count; ++i) { \
        printf(typeFmt, (da)->items[i]); \
    } \
    nl \
} while (0)

/**
 * @brief Generates a random unsigned 64-bit integer between min and max (inclusive).
 *
 * Uses the standard rand() function.
 *
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A random value in [min, max].
 */
u64 randint(u64 min, u64 max);

/**
 * @brief Generates a random unsigned 64-bit integer between min and max (inclusive) using a PRNG.
 *
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A random value in [min, max].
 */
u64 prng_randint(u64 min, u64 max);

/**
 * @brief Generates a random float in the range [0.0, 1.0].
 *
 * @return A random float value.
 */
f64 randfloat(void); // range 0..=1

#endif // UTILS_H