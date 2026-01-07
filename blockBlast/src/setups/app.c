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

    for (u64 fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        fonts[fontId] = LoadFontEx("assets/arial.ttf", fontSize, NULL, 0);
        if (!IsFontValid(fonts[fontId])) {
            log_warn("Font %zu (%d) wasn't proprely loaded", fontId, fontSize);
            return false;
        }
        
        fontSize += 2;
    }

    return true;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}

bool8 initApp(void) {
    srand(time(NULL));
    
    u64 seeds[2] = { 0 };
    plat_get_entropy(seeds, sizeof(seeds));
    prng_seed(seeds[0], seeds[1]);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    appFont = LoadFont("../assets/fonts/Nunito/Nunito-Black.ttf");
    if (!IsFontValid(appFont)) {
        log_warn("App font wasn't proprely loaded");
    }
    // if (initFonts()) return false;

    initGame();

    return true;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    freeFonts();

    CloseWindow();
}
