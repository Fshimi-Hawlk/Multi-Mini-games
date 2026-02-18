/**
 * @file app.h
 * @author Fshimi-Hawlk
 * @date 2026-01-30
 * @date 2026-02-18
 * @brief Public interface for drawing lobby UI overlay elements (skin selection menu).
 *
 * This header declares the rendering functions responsible for the skin/character
 * selection interface that appears on top of the lobby scene.
 *
 * All functions:
 *   - operate in **screen-space** coordinates
 *   - should be called during the UI render pass (after world rendering,
 *     typically inside BeginDrawing() / EndDrawing() but outside BeginMode2D())
 *   - perform no state changes — they are pure draw calls
 *   - rely on pre-loaded textures and global rectangle definitions
 *
 * Typical usage in the main render loop:
 *   BeginDrawing();
 *     ClearBackground(APP_BACKGROUND_COLOR);
 *
 *     BeginMode2D(game->cam);
 *       // world rendering: platforms, player, etc.
 *     EndMode2D();
 *
 *     // UI overlay
 *     if (game->playerVisuals.isTextureMenuOpen)
 *         drawMenuTextures(game);
 *     drawSkinButton();
 *
 *   EndDrawing();
 *
 * @see ui/app.c           for implementation
 * @see utils/globals.h    for logoSkinButton, skinButtonRect
 * @see utils/userTypes.h  for LobbyGame_St, PlayerVisuals_St
 */

#ifndef UI_APP_H
#define UI_APP_H

#include "utils/userTypes.h"

// ────────────────────────────────────────────────
// Skin selection UI rendering
// ────────────────────────────────────────────────

/**
 * @brief Draws the skin selection menu overlay when open.
 *
 * Renders:
 *   - a fallback debug circle for the default texture preview
 *   - a horizontal grid of available unlocked player textures
 *   - label text ("choose your skin :")
 *
 * Positions are hardcoded in screen space (top-left origin).
 * Only unlocked textures are shown as selectable.
 *
 * @param game  Pointer to the lobby game state (provides textures and default rect)
 */
void drawMenuTextures(const LobbyGame_St* const game);

/**
 * @brief Draws the skin-menu toggle button (usually an icon in a corner).
 *
 * Renders the pre-loaded button texture at the fixed screen-space rectangle
 * defined in globals (skinButtonRect).
 *
 * This button is always visible and serves as the entry point to open/close
 * the skin selection menu.
 */
void drawSkinButton(void);

#endif // UI_APP_H