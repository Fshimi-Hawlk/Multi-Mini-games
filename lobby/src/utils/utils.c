/**
 * @file utils.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Implementation of utility functions.
 */

#include "utils/utils.h"

Rectangle getTextureRec(const Texture* const texture) {
    return (Rectangle) {
        .width = texture->width,
        .height = texture->height
    };
}

Rectangle getPlayerCollisionBox(const Player_st* const player) {
    return (Rectangle) {
        player->position.x,
        player->position.y,
        player->radius * 2,
        player->radius * 2
    };
}

Vector2 getPlayerCenter(const Player_st* const player) {
    return (Vector2) {player->radius, player->radius};
}

u64 randint(u64 min, u64 max) {
    return min + (rand() % (max - min + 1));
}

#ifdef RAND_H

u64 prng_randint(u64 min, u64 max) {
    return min + (prng_rand() % (max - min + 1));
}

#endif

f64 randfloat(void) {
    return (f64) rand() / RAND_MAX;
}