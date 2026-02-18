/**
 * @file globals.c
 * @author LeandreB8
 * @date 2026-01-12
 * @date 2026-02-18
 * @brief Definitions of program-wide global variables.
 *
 * Contributors:
 * - LeandreB8:
 *    - Provided the initial variables and harcoded platform
 * - Fshimi-Hawlk:
 *    - Provided documentation
 *    - Moved some of the variables to dedicated struct.
 *
 * Contains shared state and constants used across the application:
 *   - Window rectangle and UI button placement
 *   - Font handles
 *   - Mini-game instance pointers (games[])
 *   - Lobby platform definitions (platforms[] + platformCount)
 *   - Default player sprite rectangle
 *   - Shared player texture array
 *   - Skin menu toggle and button texture
 *
 * The lobby level geometry (platforms) is hard-coded here.
 * Mini-game registration happens via the games[] array.
 * Skin menu visibility is controlled by isTextureMenuOpen.
 *
 * @see utils/globals.h for type definitions and extern declarations
 */

#include "utils/globals.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

Game_St* games[__gameSceneCount] = {0};

Platform_st platforms[] = {
    // Sol principal
    {{-1000, 500, 2000, 1000}, GREEN, 0},

    // Murs
    {{-1000, 0, 500, 500}, BLACK, 0},
    {{500, 0, 500, 500}, BLACK, 0},

    // Platformes
    {{-350, 400, 100, 30}, BROWN, 0.5},
    {{250, 400, 100, 30}, BROWN, 0.5},
    {{-200, 300, 100, 30}, BROWN, 0.5},
    {{100, 300, 100, 30}, BROWN, 0.5},
    {{-50, 200, 100, 30}, BROWN, 0.5},
    {{-350, 150, 100, 30}, BROWN, 0.5},
    {{250, 150, 100, 30}, BROWN, 0.5},
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect = {
    .x = WINDOW_WIDTH - 70,
    .y = WINDOW_HEIGHT / 2.0f - 25,
    .width = 50,
    .height = 50
};

Texture2D logoSkinButton;