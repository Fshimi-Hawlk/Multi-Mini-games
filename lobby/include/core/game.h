/**
    @file core/game.h
    @author Fshimi-Hawlk
    @author LeandreB8
    @date 2026-01-30
    @date 2026-03-30
    @brief Public interface for player physics and lobby core logic.
*/

#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

/**
    @brief Updates the player's position, velocity and state based on inputs and physics.
    @param game Pointer to the complete lobby game state.
    @param dt   Delta time in seconds for frame-independent movement.
*/
void updatePlayer(LobbyGame_St* const game, const f32 dt);

/**
    @brief Checks for and handles interaction with the skin selection menu.
    @param game Pointer to the lobby game state.
*/
void toggleSkinMenu(LobbyGame_St* const game);

/**
    @brief Processes mouse input to select a new skin from the open menu.
    @param game Pointer to the lobby game state.
*/
void choosePlayerTexture(LobbyGame_St* const game);

/**
    @brief Returns the axis-aligned bounding box for the player's circle collider.
*/
Rectangle getPlayerCollisionBox(const Player_St* const player);

/**
    @brief Returns the local center offset for the player character drawing.
*/
Vector2 getPlayerCenter(const Player_St* const player);

/**
    @brief Calculates the percentage of the player submerged in a water rectangle.
*/
f32 getWaterSubmersion(const Player_St* player, const Rectangle waterRect);

#endif // CORE_GAME_H
