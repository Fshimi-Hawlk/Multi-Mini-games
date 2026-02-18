/**
 * @file utils.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of utility functions for the lobby.
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
 * @brief Gets the collision box for a player.
 * @param player Pointer to the player structure.
 * @return A Rectangle representing the player's collision bounds.
 */
Rectangle getPlayerCollisionBox(const Player_st* const player) {
    return (Rectangle) {
        player->position.x,
        player->position.y,
        player->radius * 2,
        player->radius * 2
    };
}

/**
 * @brief Gets the center position of a player.
 * @param player Pointer to the player structure.
 * @return A Vector2 representing the player's center.
 */
Vector2 getPlayerCenter(const Player_st* const player) {
    return (Vector2) {player->radius, player->radius};
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