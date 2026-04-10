/**
    @file core/game.h
    @author Fshimi-Hawlk
    @date 2026-01-30
    @date 2026-02-23
    @brief Core logic and helper functions for the lobby gameplay (player update, collision, skin selection).

    This header declares the main functions that drive:
        - player physics and input handling
        - circle-vs-rectangle collision resolution
        - skin / texture selection logic
        - skin menu toggle

    Functions are split between:
        - pure helpers (getPlayerCollisionBox, getPlayerCenter) — used by rendering
        - update / simulation logic (updatePlayer, resolveCircleRectCollision)
        - UI interaction logic (choosePlayerTexture, toggleSkinMenu)

    All update functions expect dt in seconds (typically GetFrameTime()).
    Collision functions assume circle-based player representation.

    Typical usage in the lobby main loop:
      updatePlayer(&game->player, platforms, platformCount, GetFrameTime());
      toggleSkinMenu(game);
      choosePlayerTexture(&game->player, game);

    @see `core/game.c`        for implementation details
    @see `utils/userTypes.h`  for Player_St, LobbyGame_St, Platform_St definitions
    @see `utils/globals.h`    for skinButtonRect (used in toggleSkinMenu)
 */

#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

// ------------------------------------------------
// Player shape / rendering helpers
// ------------------------------------------------

/**
    @brief Computes the axis-aligned bounding box that fully encloses the player's circle.

    Used primarily by rendering code (DrawTexturePro destination rectangle)
    and potentially by broad-phase checks.

    @param player  Pointer to player state (uses position and radius)
    @return Rectangle centered on player->position with width/height = 2 radius
 */
Rectangle lobby_getPlayerCollisionBox(const Player_St* const player);

/**
    @brief Returns the local offset from the top-left of the collision box to its center.

    Equivalent to {player->radius, player->radius}.
    Used as the origin/pivot point in DrawTexturePro calls.

    @param player  Pointer to player state
    @return Vector2 {radius, radius}
 */
Vector2 lobby_getPlayerCenter(const Player_St* const player);

// ------------------------------------------------
// Player physics & update
// ------------------------------------------------

/**
    @brief Updates player position, velocity, angle, jump state, and performs collisions.

    Handles:
        - horizontal input + friction
        - gravity
        - rotation based on movement direction
        - jump buffering + coyote time + limited air jumps
        - collision resolution against all platforms

    @param player       Player state to modify
    @param platforms    Array of static platforms
    @param nbPlatforms  Number of platforms
    @param dt           Delta time in seconds
 */
void lobby_updatePlayer(Player_St* const player, const Platform_St* const platforms, const int nbPlatforms, const float dt);

// ------------------------------------------------
// Skin / texture selection
// ------------------------------------------------

/**
    @brief Handles skin selection via mouse clicks on preview rectangles or number keybinds.

    Checks mouse clicks inside the skin menu grid (only unlocked textures)  
    or key presses (1,2,3...) and applies the selected texture if unlocked.
    Closes the menu on successful selection.

    @param player  Player whose textureId will be updated
    @param game    Lobby game state (to close the menu via playerVisuals)
 */
void lobby_choosePlayerTexture(Player_St* player, LobbyGame_St* const game);

/**
    @brief Toggles the skin selection menu visibility.

    Opens/closes when:
        - left mouse button is pressed on skinButtonRect, or
        - P key is pressed

    @param game  Lobby game state (modifies playerVisuals.isTextureMenuOpen)
 */
void lobby_toggleSkinMenu(LobbyGame_St* const game);

#endif // CORE_GAME_H
