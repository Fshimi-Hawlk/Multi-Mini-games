/**
    @file app.c
    @author Fshimi Hawlk
    @date 2026-03-02
    @date 2026-03-05
    @brief Application setup and teardown.
*/

#include "utils/globals.h"

#include "setups/app.h"

bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    fonts[fontId] = LoadFontEx(ASSET_PATH "fonts/Noto/static/NotoSansMono-Bold.ttf", fontSize, NULL, 0);
        if (!IsFontValid(fonts[fontId])) {
            log_warn("Font %zu (%d) wasn't proprely loaded", fontId, fontSize);
            return false;
        }

    return true;
}

bool initFonts(void) {
    u64 fontSize = 4;
    bool allFontLoaded = true;
    
    for (u64 fontId = 0; allFontLoaded && fontId < __fontSizeCount; fontId++) {
        allFontLoaded = loadFontIdForSize(fontId, fontSize);
        fontSize += 2;
    }

    return allFontLoaded;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < __fontSizeCount; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}

bool initApp(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

#ifdef LOGGER_H
    init_logger();
#endif

    if (!initFonts()) {
        log_warn("Couldn't initialize every fonts");
    };

    return true;
}

void freeApp(void) {
    // arena_free(&globalArena);
    // arena_free(&tempArena);

    freeFonts();

    CloseWindow();
}
