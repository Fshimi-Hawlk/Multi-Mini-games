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
#include "APIs/chatAPI.h"

/** @brief Main window rectangle (set at init). */
Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
/** @brief Array of loaded fonts in increasing sizes. */
Font      fonts[__fontSizeCount] = {0};
/** @brief Primary font for UI text. */
Font      appFont = {0};

/** @brief Array of platforms in the lobby. */
Platform_St platforms[] = {
    {{-1000, 500, 2000, 1000}, {0, 228, 48, 255}, 0},
    {{-1000, 0, 500, 500}, {0, 0, 0, 255}, 0},
    {{500, 0, 500, 500}, {0, 0, 0, 255}, 0},
    {{-350, 400, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{250, 400, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-200, 300, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{100, 300, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-50, 200, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{-350, 150, 100, 30}, {127, 106, 79, 255}, 0.5},
    {{250, 150, 100, 30}, {127, 106, 79, 255}, 0.5},
};
/** @brief Number of platforms in the lobby. */
u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect = {
    .x = WINDOW_WIDTH - 70,
    .y = WINDOW_HEIGHT / 2.0f - 25,
    .width = 50,
    .height = 50
};

/** @brief Trigger zone for the King For Four game. */
Rectangle kingForFourZone = {
    -350, 400 - 60, 100, 60
};

/** @brief Flag indicating if the texture selection menu is open. */
bool isTextureMenuOpen = false;

/** @brief Texture for the skin button logo. */
Texture2D logoSkinButton;
ChatState_St g_chatState = {0};
