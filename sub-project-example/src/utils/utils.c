/**
 * @file utils.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Implementation of utility functions.
 */

#include "utils/utils.h"

u64 randint(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

// #ifndef _USE_DEFAULT_RAND
// u64 prng_randint(u64 min, u64 max) {
//     return min + (prng_rand() % (max - min + 1));
// }
// #endif

f64 randfloat(void) {
    return (f64) rand() / RAND_MAX;
}