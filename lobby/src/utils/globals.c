/**
    @file utils/globals.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Definitions of program-wide global variables.

    Contains shared state and constants used across the application:
        - Window rectangle and UI button placement
        - Font handles
        - Lobby platform definitions (platforms[] + platformCount)
        - Game zone definitions (gameZones[] + gameZoneCount)
        - Default player sprite rectangle
        - Shared player texture array
        - Skin menu toggle and button texture

    The lobby level geometry (platforms) and game zones are hard-coded here.

    @see `utils/globals.h` for type definitions and extern declarations
*/

#include "utils/globals.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

Platform_st platforms[] = {
    // Ground and walls
    {{-1000, 500, 2000, 1000}, {0, 228, 48, 255}, 0},      // Main ground
    {{-1000, 0, 500, 500}, {0, 0, 0, 255}, 0},             // Left wall
    {{500, 0, 500, 500}, {0, 0, 0, 255}, 0},               // Right wall
    
    // Floating platforms
    {{-350, 400, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{250, 400, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-200, 300, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{100, 300, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-50, 200, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-350, 150, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{250, 150, 100, 30}, {127, 106, 79, 255}, 0.5},
    
    // Elevated platforms near bowling zone
    {{450, 50, 150, 30}, {100, 149, 237, 255}, 0.3},      // Platform leading to bowling
    {{-550, 100, 150, 30}, {100, 149, 237, 255}, 0.3},    // Platform on left side
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

GameZone_st gameZones[] = {
    {
        .hitbox = {
            .x = GAME_ZONE_BOWLING_X,
            .y = GAME_ZONE_BOWLING_Y,
            .width = GAME_ZONE_BOWLING_WIDTH,
            .height = GAME_ZONE_BOWLING_HEIGHT
        },
        .targetScene = GAME_SCENE_BOWLING,
        .debugColor = {0, 191, 255, 255},  // Deep sky blue
        .isHighlighted = false
    },
};

u32 gameZoneCount = sizeof(gameZones) / sizeof(gameZones[0]);

Rectangle skinButtonRect = {
    .x = WINDOW_WIDTH - 70,
    .y = WINDOW_HEIGHT / 2.0f - 25,
    .width = 50,
    .height = 50
};

Texture2D logoSkinButton;
