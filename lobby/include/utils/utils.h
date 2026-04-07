/**
    @file utils/utils.h
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-03-30
    @brief General-purpose macros, inline helpers and small utility functions.
*/

#ifndef UTILS_H
#define UTILS_H

#include "common.h"

// 
// Macros
// 

#define swap(T, a, b) do { T _tmp = (a); (a) = (b); (b) = _tmp; } while (0)

#define ARRAY_LEN(x) (u32)(sizeof(x) / sizeof((x)[0]))

#define clamp(val, min, max) \
    ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
    @brief In-place array shuffle (Fisher-Yates).
*/
#define shuffleArray(T, array, size, randFunc) \
do { \
    if ((size) > 1) { \
        for (u32 i = (u32)(size) - 1; i > 0; --i) { \
            u32 r = randFunc() % (i + 1); \
            if (r == i) continue; \
            swap(T, array[i], array[r]); \
        } \
    } \
} while (0)

// 
// Functions
// 

/**
    @brief Returns the full source rectangle for a texture.
*/
Rectangle getTextureRec(Texture2D texture);

/**
    @brief Returns the center position of a rectangle.
*/
Vector2 getRectCenterPos(Rectangle rect);

/**
    @brief Platform-specific entropy gathering for PRNG seeding.
*/
void plat_get_entropy(void* buffer, u32 size);

/**
    @brief Random int in [min, max].
*/
u64 prng_randint(u64 min, u64 max);

/**
    @brief Random float in [0, 1].
*/
f64 randfloat(void);

#endif // UTILS_H
