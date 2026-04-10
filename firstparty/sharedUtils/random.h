/**
    @file firstparty/utils/random.h
    @author Fshimi-Hawlk
    @date 2026-04-03
    @date 2026-04-03
    @brief Random number generation and array shuffling utilities.
*/

#ifndef FIRSTPARTY_UTILS_RANDOM_H
#define FIRSTPARTY_UTILS_RANDOM_H

#include "baseTypes.h"
#include "sharedUtils/mathUtils.h"

/**
    @brief Shuffles an array in-place using XOR swap and Fisher-Yates algorithm.
*/
#define shuffleArrayXor(array, count, randFn) \
do { \
    for (u32 i = count - 1; i > 0; --i) { \
        u32 r = randFn() % (i + 1); \
        if (r == i) continue; \
        swapXor(array[i], array[r]); \
    } \
} while (0)

/**
    @brief Shuffles an array in-place using temporary variable swap and Fisher-Yates algorithm.
*/
#define shuffleArrayT(T, array, count, randFn) \
do { \
    for (u32 i = count - 1; i > 0; --i) { \
        u32 r = randFn() % (i + 1); \
        if (r == i) continue; \
        swapT(T, array[i], array[r]); \
    } \
} while (0)

/**
    @brief Shuffles a dynamic array's items using XOR swap.
*/
#define da_shuffleXor(da, randFn) shuffleArrayXor((da)->items, (da)->count, (randFn))

/**
    @brief Shuffles a dynamic array's items using Type swap.
*/
#define da_shuffleT(T, da, randFn) shuffleArrayT(T, (da)->items, (da)->count, (randFn))

/**
    @brief Generates a random unsigned 64-bit integer between min and max (inclusive).

    Uses the standard rand() function.

    @param min The minimum value.
    @param max The maximum value.
    @return A random value in [min, max].
*/
u64 randint(u64 min, u64 max);

#ifdef RAND_H
/**
    @brief Generates a random unsigned 64-bit integer between min and max (inclusive) using a PRNG.

    @param min The minimum value.
    @param max The maximum value.
    @return A random value in [min, max].
*/
u64 prng_randint(u64 min, u64 max);
#endif

/**
    @brief Generates a random float in the range [0.0, 1.0].

    @return A random float value.
*/
f64 randfloat(void); // range 0..=1

#endif // FIRSTPARTY_UTILS_RANDOM_H