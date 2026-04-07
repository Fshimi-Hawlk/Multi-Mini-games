/**
    @file app.c
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Application setup and teardown.
*/

#include "utils/globals.h"
#include "utils/audio.h"

#include "setups/app.h"
#include "setups/game.h"

#include "utils/globals.h"


bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    polyBlast_fonts[fontId] = LoadFontEx(FONT_PATH "Nunito/Nunito-Black.ttf", fontSize, NULL, 0);
    if (!IsFontValid(polyBlast_fonts[fontId])) {
        log_warn("Font %zu (%f) wasn't proprely loaded", fontId, fontSize);
        return false;
    }

    return true;
}

bool initFonts(void) {
    u64 fontSizes[__fontSizeCount] = {16, 24, 32, 48, 64, 96, 128};
    bool allFontLoaded = true;
    
    for (u64 fontId = 0; allFontLoaded && fontId < __fontSizeCount; fontId++) {
        allFontLoaded = loadFontIdForSize(fontId, fontSizes[fontId]);
    }

    return allFontLoaded;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < __fontSizeCount; fontId++) {
        UnloadFont(polyBlast_fonts[fontId]);
    }
}

bool initApp(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    blockBlast_initAudio();

    initLogger();

    if (!initFonts()) {
        log_warn("Couldn't initialize every fonts");
    };

    initGame(&mainGameState);

    return true;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    freeFonts();

    blockBlast_freeAudio();

    CloseWindow();
}
