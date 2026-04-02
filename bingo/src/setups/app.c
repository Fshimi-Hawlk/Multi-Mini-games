/**
    @file app.c
    @author Fshimi Hawlk
    @date 2026-03-02
    @date 2026-03-16
    @brief Application setup and teardown.
*/

<<<<<<< HEAD
#include "utils/globals.h"
#include "utils/common.h"

#include "setups/app.h"

bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    bingo_fonts[fontId] = LoadFontEx(ASSET_PATH "fonts/Noto/static/NotoSansMono-Bold.ttf", fontSize, NULL, 0);
=======
#include "setups/app.h"

#include "utils/globals.h"

static bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    bingo_fonts[fontId] = LoadFontEx(FONT_PATH "Noto/static/NotoSansMono-Bold.ttf", fontSize, NULL, 0);
>>>>>>> origin/mgit-PR1-20-03
        if (!IsFontValid(bingo_fonts[fontId])) {
            log_warn("Font %zu (%f) wasn't proprely loaded", fontId, fontSize);
            return false;
        }

    return true;
}

bool bingo_initFonts(void) {
    u64 fontSize = 4;
    bool allFontLoaded = true;
    
    for (u64 fontId = 0; allFontLoaded && fontId < __fontSizeCount; fontId++) {
        allFontLoaded = loadFontIdForSize(fontId, fontSize);
        fontSize += 2;
    }

    return allFontLoaded;
}

void bingo_freeFonts(void) {
    for (u64 fontId = 0; fontId < __fontSizeCount; fontId++) {
        UnloadFont(bingo_fonts[fontId]);
    }
}

<<<<<<< HEAD
bool initApp(void) {
    u64 seeds[2] = { 0 };
    plat_get_entropy(seeds, sizeof(seeds));
    prng_seed(seeds[0], seeds[1]);

=======
bool bingo_initApp(void) {
    srand(time(NULL));
>>>>>>> origin/mgit-PR1-20-03
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

#ifdef LOGGER_H
    init_logger();
#endif

    if (!bingo_initFonts()) {
        log_warn("Couldn't initialize every bingo_fonts");
    };

    return true;
}

void bingo_freeApp(void) {
    // arena_free(&globalArena);
    // arena_free(&tempArena);

    bingo_freeFonts();

    CloseWindow();
}
