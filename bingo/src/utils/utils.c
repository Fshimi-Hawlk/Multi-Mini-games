/**
    @file utils/utils.c
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-02-23
    @brief Implementation of small, reusable utility functions for geometry, random numbers and player-related calculations.

    Contributors:
        - Fshimi-Hawlk:
            - Added documentation
            - Added `randint`, `randfloat` and `prgn_randint`
            - Added `getTextureRec`
        - <Name>:
            - ...

    This file contains lightweight helper functions that are used across multiple parts of the game:
        - texture rectangle creation
        - simple (and optionally cryptographically stronger) random number generation

    @note: For `rand_prng` in `common.h`, `rand.h` should be uncommented

    @see `utils/utils.h` for function declarations and type definitions
*/

#include "utils/utils.h"

Rectangle getTextureRec(const Texture texture) {
    return (Rectangle) {
        .width = texture.width,
        .height = texture.height
    };
}

u64 randint(u64 min, u64 max) {
    if (max <= min) return min;
    u64 range = max - min + 1;
    // Use 32-bit random for ranges up to 2^32, or combine for larger
    if (range <= 0xFFFFFFFF) {
        return min + (prng_rand() % range);
    } else {
        u64 large_rand = ((u64)prng_rand() << 32) | (u64)prng_rand();
        return min + (large_rand % range);
    }
}

#ifdef RAND_H

u64 prng_randint(u64 min, u64 max) {
    return randint(min, max);
}

#endif

f64 randfloat(void) {
    return (f64) prng_randf();
}