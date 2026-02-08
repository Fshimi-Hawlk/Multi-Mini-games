/**
 * @file app.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Application setup and teardown.
 */

#include "utils/globals.h"
#include "setups/app.h"
#include "setups/game.h"

bool8 initFonts(void) {
    u64 fontSize = 8;

    bool8 allFontLoaded = true;

    for (u64 fontId = 0; fontId < _fontSizeCount; fontId++) {
        fonts[fontId] = LoadFontEx("../assets/fonts/Nunito/Nunito-Black.ttf", fontSize, NULL, 0);
        if (!IsFontValid(fonts[fontId])) {
            log_warn("Font %zu (%d) wasn't proprely loaded", fontId, fontSize);
            allFontLoaded = false;
        }

        fontSize += 2;
    }

    return allFontLoaded;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < _fontSizeCount; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}

bool8 initApp(void) {
    srand(time(NULL));

// if you want to use another rand generator system
// #ifndef _USE_DEFAULT_RAND
//     u64 seeds[2] = { 0 };
//     plat_get_entropy(seeds, sizeof(seeds));
//     prng_seed(seeds[0], seeds[1]);
// #endif

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    if (!initFonts()) {
        log_warn("Couldn't initialize every fonts");
    };

    initGame();

    return true;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    freeFonts();

    CloseWindow();
}
