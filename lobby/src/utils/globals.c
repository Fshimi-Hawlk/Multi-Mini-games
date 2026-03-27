/**
    @file utils/globals.c
    @author LeandreB8
    @author Fshimi-Hawlk
    @author i-Charlys (CAILLON Charles)
    @date 2026-01-12
    @date 2026-03-18
    @brief Definitions of program-wide global variables used in the lobby.

    Contributors:
        - LeandreB8:
            - Provided the initial variables and harcoded platform
        - Fshimi-Hawlk:
            - Provided documentation
            - Moved some of the variables to dedicated struct.

    Contains shared state and constants used across the application:
        - Window rectangle and UI button placement
        - Font handles
        - Mini-game instance pointers (games[])
        - Lobby platform definitions (platforms[] + platformCount)
        - Default player sprite rectangle
        - Shared player texture array
        - Skin menu toggle and button texture

    The lobby level geometry (platforms) is hard-coded here.
    Mini-game registration happens via the games[] array.
    Skin menu visibility is controlled by isTextureMenuOpen.

    @see `utils/globals.h` for type definitions and extern declarations
*/

#include "utils/globals.h"

/** @brief Array of loaded fonts in increasing sizes. */
Font lobby_fonts[__fontSizeCount] = {0};

/**
    @brief Complete lobby terrain layout.
    - Left side: isolated test levels (one wide platform per terrain type, stacked vertically).
    - Bottom level on left = water pool as requested.
    - Center/main ground connects left test area to right side.
    - Right side: all interaction zones in a tight horizontal line (200-unit spacing).
    - Large solid platform directly under the interaction zones.
    - Three example portals placed for future teleport testing.
    @note Player starts at (0.0f, 0.0f). Raylib coordinates: negative Y = up, positive Y = down.
*/
TerrainVec_St terrains = {0};

GameInteractionZone_St gameInteractionZones[__miniGameCount] = {
    // All interaction zones now in a tight horizontal line (exactly 200-unit spacing)
    // Placed above the large stone platform for easy access
    [MINI_GAME_BATTLESHIP] = { .hitbox = { .x = 700.0f,  .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Battleship" },
    [MINI_GAME_BINGO]      = { .hitbox = { .x = 900.0f,  .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Bingo" },
    [MINI_GAME_CONNECT_4]  = { .hitbox = { .x = 1100.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Connect 4" },
    [MINI_GAME_KFF]        = { .hitbox = { .x = 1300.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "King For Four" },
    [MINI_GAME_MINIGOLF]   = { .hitbox = { .x = 1500.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Mini Golf" },
    [MINI_GAME_MORPION]    = { .hitbox = { .x = 1700.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Morpion" },
    [MINI_GAME_OTHELLO]    = { .hitbox = { .x = 1900.0f, .y = 175.0f, .width = 75.0f, .height = 75.0f }, .name = "Othello" }
};

Rectangle skinButtonRect = {0};

/** @brief Texture for the skin button logo. */
Texture2D logoSkinButton;