/**
    @file utils/globals.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-02-23
    @brief Definitions of program-wide global variables.

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

Font      fonts[_fontSizeCount] = {0};

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

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect;

Texture2D logoSkinButton;
