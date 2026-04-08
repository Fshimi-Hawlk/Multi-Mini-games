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

#include "include/ui/paramsMenu.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

Platform_st platforms[] = {
    /* Sol principal */
    {{-1600, 500, 3200, 1000}, {34, 139, 34, 255}, 0},
    /* Bas du tronc – zigzag */
    {{ -100, 400, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  100, 280, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{ -280, 160, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Grosses branches du bas */
    {{  -50,  50, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{ -480,  60, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  280,  40, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Milieu de l'arbre */
    {{ -350, -60, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  150, -80, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{ -150,-170, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Extérieurs hauts */
    {{ -520,-220, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  350,-240, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Vers la canopée */
    {{ -280,-340, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{   80,-360, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{ -100,-460, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Branches supérieures */
    {{ -380,-520, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  250,-550, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{ -220,-640, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{   50,-660, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Sommet du feuillage */
    {{ -320,-760, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  180,-780, 200, 20}, {139, 90, 43, 255}, 0.3f},
    {{  -80,-880, 200, 20}, {139, 90, 43, 255}, 0.3f},
    /* Cime absolue */
    {{ -100,-1000,200, 20}, {139, 90, 43, 255}, 0.3f},
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect = {
    /* FIX: This initial value uses WINDOW_WIDTH/HEIGHT (800/600) but the lobby
     * actually runs at 1200×800 and can be resized. The rect is recalculated
     * dynamically every frame via skinButtonRect_get() in ui/app.c and
     * core/game.c; this static value is only used before the first frame. */
    .x = WINDOW_WIDTH - 70,
    .y = WINDOW_HEIGHT / 2.0f - 25,
    .width = 50,
    .height = 50
};

Texture2D logoSkinButton;

// Parameters menu state
ParamsMenu_St paramsMenu = {0};

// Visual / atmospheric globals
const Vector2 moonLightDir = {-0.6f, -0.8f};
Texture2D texTree = {0};
Texture2D texBackground = {0};
Texture2D platformTextures[2] = {0};
float gameTime = 0.0f;
GrassBlade_st grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

// Audio
Sound sound_jump       = {0};
Sound sound_doubleJump = {0};
Sound sound_gameLaunch = {0};
Sound meme             = {0};
