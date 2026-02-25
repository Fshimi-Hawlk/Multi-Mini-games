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
        - Moved `getTextureRec` here

    This file contains lightweight helper functions that are used across multiple parts of the game:
        - texture and collision rectangle creation
        - player position/center/collision utilities
        - simple (and optionally cryptographically stronger) random number generation

    @see `utils/utils.h` for function declarations and type definitions
*/

#include "utils/utils.h"

/**
 * @brief Gets a source rectangle from a texture.
 * @param texture Pointer to the texture.
 * @return A Rectangle covering the entire texture.
 */
Rectangle getTextureRec(const Texture texture) {
    return (Rectangle) {
        .width = texture.width,
        .height = texture.height
    };
}

/**
 * @brief Generates a random unsigned 64-bit integer between min and max (inclusive).
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A random value in [min, max].
 */
u64 randint(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

#ifdef RAND_H

/**
 * @brief Generates a random unsigned 64-bit integer between min and max (inclusive) using a PRNG.
 * @param min The minimum value.
 * @param max The maximum value.
 * @return A random value in [min, max].
 */
u64 prng_randint(u64 min, u64 max) {
    return min + (prng_rand() % (max - min + 1));
}

#endif

/**
 * @brief Generates a random float in the range [0.0, 1.0].
 * @return A random float value.
 */
f64 randfloat(void) {
    return (f64) rand() / RAND_MAX;
}
