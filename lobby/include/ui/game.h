/**
    @file ui/game.h
    @author Fshimi-Hawlk
    @date 2026-01-30
    @date 2026-04-10
    @brief Public interface for low-level drawing functions used in the lobby scene.

    This header declares the core rendering helpers responsible for visualizing
    the main gameplay elements of the lobby (player character and platforms).

    All functions:
        - expect world-space coordinates
        - must be called inside an active BeginMode2D() block
        - perform no state changes beyond Raylib draw calls
        - are intended as leaf-level primitives (no logic, no side-effects)

    Typical usage pattern in the lobby render loop:
    BeginMode2D(game->cam); {
        drawPlatforms(platforms, platformCount);
        drawPlayer(game, &game->player);
        // other world-space elements (particles, effects, etc.)
    } EndMode2D();

    @see `ui/game.c`          for implementation
    @see `utils/userTypes.h`  for LobbyGame_St, Player_St, Platform_St definitions
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

void lobby_drawPlatforms(const Platform_St *const platforms, const int count);

/**
    @brief Draws the lobby's terrains.
*/
void lobby_drawTerrains(void);

void lobby_drawTree(void);
void lobby_drawWorldBoundaries(const Player_St* const player);

void lobby_drawGameZones(const Player_St* const player);


#endif // UI_GAME_H
