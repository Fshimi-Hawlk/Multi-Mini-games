/**
    @file utils/utils.c
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-03-30
    @brief Implementation of small utility functions.
*/

#include "utils/utils.h"

Rectangle getTextureRec(const Texture2D texture) {
    return (Rectangle) { 0, 0, (f32)texture.width, (f32)texture.height };
}

Vector2 getRectCenterPos(Rectangle rect) {
    return (Vector2){rect.x + rect.width / 2.0f, rect.y + rect.height / 2.0f};
}

u64 randint(u64 min, u64 max) {
    if (max <= min) return min;
    u64 range = max - min + 1;
    u64 large_rand;
    u64 limit = 0xFFFFFFFFFFFFFFFFULL - (0xFFFFFFFFFFFFFFFFULL % range);
    do {
        large_rand = ((u64)prng_rand() << 32) | (u64)prng_rand();
    } while (large_rand >= limit);
    return min + (large_rand % range);
}

u64 prng_randint(u64 min, u64 max) {
    return randint(min, max);
}

f64 randfloat(void) {
    return (f64) prng_randf();
}
