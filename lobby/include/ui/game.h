/**
    @file game.h
    @author Léandre BAUDET
    @date 2026-01-30
    @date 2026-04-14
    @brief Public interface for low-level drawing functions used in the lobby scene.
*/
#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

// ────────────────────────────────────────────────
// World-space rendering primitives
// ────────────────────────────────────────────────

/**
    @brief Renders a player character at its current position with its assigned skin.

    Renders either:
        - a simple debug circle (when using PLAYER_TEXTURE_DEFAULT)
        - or a textured sprite with rotation (for custom skins)

    @param visuals  Pointer to the collection of pre-loaded player textures.
    @param player   Pointer to the player state (position, textureId, active).
*/
void lobby_drawPlayer(const PlayerVisuals_St* const visuals, const Player_St* const player);

/**
    @brief Draws the lobby's terrains.
*/
void lobby_drawTerrains(void);

/**
    @brief Description for lobby_drawTree
*/
void lobby_drawTree(void);
/**
    @brief Description for lobby_drawWorldBoundaries
    @param[in,out] player The player parameter
*/
void lobby_drawWorldBoundaries(const Player_St* const player);

/**
    @brief Description for lobby_drawGameZones
    @param[in,out] player The player parameter
*/
void lobby_drawGameZones(const Player_St* const player);


#endif // UI_GAME_H
