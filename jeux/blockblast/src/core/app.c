/**
    @file app.c
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Application setup and teardown.
*/

#include "utils/common.h"
#include "utils/globals.h"

#ifndef ASSET_PATH
#define ASSET_PATH "assets/"
#endif

#include "core/app.h"
#include "core/game.h"
#include "utils/audio.h"

bool initFonts(void) {
    for (u64 fontId = 0; fontId < _fontSizeCount; fontId++) {
        fonts[fontId] = GetFontDefault();
    }
    return true;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < _fontSizeCount; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}

bool initApp(void) {
    srand(time(NULL));

#ifndef _USE_DEFAULT_RAND
    u64 seeds[2] = { 0 };
    plat_get_entropy(seeds, sizeof(seeds));
    prng_seed(seeds[0], seeds[1]);
#endif

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    blockBlast_initAudio();

#ifdef LOGGER_H
    init_logger();
#endif

    if (!initFonts()) {
        log_warn("Couldn't initialize every fonts");
    };

    initGame(&mainGameState, false);

    return true;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    freeFonts();

    blockBlast_freeAudio();

    CloseWindow();
}
