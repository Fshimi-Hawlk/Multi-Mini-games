/**
 * @file game.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief UI functions for rendering game elements in the lobby.
 */

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
 * @brief Renders the player to the screen.
 * @param player Pointer to the player structure to draw.
 */
void drawPlayer(const LobbyGame_St* const game);

/**
 * @brief Renders the platforms to the screen.
 * @param platforms Array of platforms to draw.
 * @param nbPlatforms Number of platforms in the array.
 */
void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms);

#endif // UI_GAME_H