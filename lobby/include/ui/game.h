/**
 * @file game.h
 * @author Fshimi-Hawlk
 * @date 2026-01-30
 * @date 2026-02-18
 * @brief Public interface for low-level drawing functions used in the lobby scene.
 *
 * This header declares the core rendering helpers responsible for visualizing
 * the main gameplay elements of the lobby (player character and platforms).
 *
 * All functions:
 *   - expect world-space coordinates
 *   - must be called inside an active BeginMode2D() block
 *   - perform no state changes beyond Raylib draw calls
 *   - are intended as leaf-level primitives (no logic, no side-effects)
 *
 * Typical usage pattern in the lobby render loop:
 *   BeginMode2D(game->cam);
 *   drawPlatforms(platforms, platformCount);
 *   drawPlayer(game, &game->player);
 *   // other world-space elements (particles, effects, etc.)
 *   EndMode2D();
 *
 * @see ui/game.c          for implementation
 * @see utils/userTypes.h  for LobbyGame_St, Player_st, Platform_st definitions
 */

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

// ────────────────────────────────────────────────
// World-space rendering primitives
// ────────────────────────────────────────────────

/**
 * @brief Draws the player character in the lobby.
 *
 * Renders either:
 *   - a simple debug circle (when using PLAYER_TEXTURE_DEFAULT)
 *   - or a textured sprite with rotation (for custom skins)
 *
 * Uses helper functions getPlayerCollisionBox() and getPlayerCenter()
 * to compute destination rectangle and pivot point.
 *
 * @param game      Pointer to the full lobby game state (provides textures and visuals)
 * @param player    Pointer to the player state (position, radius, textureId, angle, etc.)
 */
void drawPlayer(const LobbyGame_St* const game, const Player_st* const player);

/**
 * @brief Draws all static platforms in the lobby world.
 *
 * Renders each platform as a rounded rectangle using its stored rect, color and roundness.
 *
 * @param platforms    Array of platform definitions
 * @param nbPlatforms  Number of platforms in the array
 */
void drawPlatforms(const Platform_st* const platforms, const int nbPlatforms);

#endif // UI_GAME_H