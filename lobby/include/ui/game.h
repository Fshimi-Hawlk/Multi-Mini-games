/**
    @file ui/game.h
    @author Fshimi-Hawlk
    @date 2026-01-30
    @date 2026-03-30
    @brief Public interface for low-level drawing functions used in the lobby scene.
*/

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
    @brief Renders a player character at its current position with its assigned skin.
    @param playerVisuals Pointer to the collection of pre-loaded player textures.
    @param player        Pointer to the player state (position, textureId, active).
*/
void drawPlayer(const PlayerVisuals_St* const playerVisuals, const Player_St* const player);

/**
    @brief Draws the lobby world background (sky, mountains).
*/
void drawLobbyBackground(void);

/**
    @brief Draws the entire lobby world.
*/
void drawLobbyTerrains(void);

#endif // UI_GAME_H
