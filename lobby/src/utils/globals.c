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

<<<<<<< HEAD
=======
#define PARAMS_MENU_IMPLEMENTATION
#include "../../firstparty/include/ui/paramsMenu.h"

>>>>>>> 3777fd6 (- add : new 3D golf game)
Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

Platform_st platforms[] = {
    /* Sol principal */
    {{-1000, 500, 2000, 1000}, {34, 139, 34, 255}, 0},
    /* Murs latéraux (pierre) */
    {{-1000, 0, 500, 500}, {80, 70, 60, 255}, 0},
    {{500, 0, 500, 500}, {80, 70, 60, 255}, 0},
    /* Plateformes bois */
    {{-350, 400, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{250, 400, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{-200, 300, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{100, 300, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{-50, 200, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{-350, 150, 100, 30}, {139, 90, 43, 255}, 0.3},
    {{250, 150, 100, 30}, {139, 90, 43, 255}, 0.3},
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect = {
<<<<<<< HEAD
=======
    /* FIX: This initial value uses WINDOW_WIDTH/HEIGHT (800/600) but the lobby
     * actually runs at 1200×800 and can be resized. The rect is recalculated
     * dynamically every frame via skinButtonRect_get() in ui/app.c and
     * core/game.c; this static value is only used before the first frame. */
>>>>>>> 3777fd6 (- add : new 3D golf game)
    .x = WINDOW_WIDTH - 70,
    .y = WINDOW_HEIGHT / 2.0f - 25,
    .width = 50,
    .height = 50
};

Texture2D logoSkinButton;
<<<<<<< HEAD
=======

// Parameters menu state
ParamsMenu_St paramsMenu = {0};
>>>>>>> 3777fd6 (- add : new 3D golf game)
