/**
 * @file game.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Header file for game logic in the lobby.
 */

#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

/**
 * @brief Updates the player's position and state based on platforms and delta time.
 * @param player Pointer to the player structure to update.
 * @param platforms Array of platforms for collision detection.
 * @param nbPlatforms Number of platforms in the array.
 * @param dt Delta time since the last frame.
 */
void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const float dt);

/**
 * @brief Resolves collision between a circular player and a rectangular obstacle.
 * @param player Pointer to the player structure.
 * @param rect The rectangle to check collision against.
 */
void resolveCircleRectCollision(Player_st* const player, const Rectangle rect);

/**
 * @brief Cycles through available player textures.
 * @param player Pointer to the player structure.
 */
void choosePlayerTexture(Player_st* const player);

/**
 * @brief Toggles the skin selection menu.
 */
void toggleSkinMenu(void);

/**
 * @brief Checks if the player has triggered a game transition zone.
 * @param player Pointer to the player structure.
 * @return 1 if a trigger is activated, 0 otherwise.
 */
int checkGameTrigger(Player_st* player);

#endif