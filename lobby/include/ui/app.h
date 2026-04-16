/**
    @file app.h
    @author Léandre BAUDET
    @date 2026-01-30
    @date 2026-04-14
    @brief Public interface for drawing lobby UI overlay elements (skin selection menu).
*/
#ifndef UI_APP_H
#define UI_APP_H

#include "utils/userTypes.h"

// ────────────────────────────────────────────────
// Skin selection UI rendering
// ────────────────────────────────────────────────

/**
    @brief Draws the skin selection menu overlay when open.

    Renders:
        - a fallback debug circle for the default texture preview
        - a horizontal grid of available unlocked player textures
        - label text ("choose your skin :")

    Positions are hardcoded in screen space (top-left origin).
    Only unlocked textures are shown as selectable.

    @param game  Pointer to the lobby game state (provides textures and default rect)
*/
void lobby_drawMenuTextures(const LobbyGame_St* const game);

/**
    @brief Draws the skin-menu toggle button (usually an icon in a corner).

    Renders the pre-loaded button texture at the fixed screen-space rectangle
    defined in globals (skinButtonRect).

    This button is always visible and serves as the entry point to open/close
    the skin selection menu.
*/
void lobby_drawSkinButton(void);

/**
    @brief Draws the real-time physics debug panel (toggle with F2).
*/
void lobby_drawPhysicsDebugPanel(LobbyGame_St* const game);

/**
    @brief Handles keyboard input for live editing of physics constants in the debug panel.
*/
void lobby_updatePhysicsDebugPanel(LobbyGame_St* const game);

/**
    @brief Recomputes all screen-space UI rectangles when the window is resized.

    Call this whenever the window size changes (in the main loop after WindowShouldClose check).
    It updates:
      - skinButtonRect
      - defaultTextureRect (for the skin menu)
      - physics debug panel position
      - (future: connection screen elements, editor panels, etc.)
*/
void lobby_updateUIOnResize(void);

#endif // UI_APP_H
