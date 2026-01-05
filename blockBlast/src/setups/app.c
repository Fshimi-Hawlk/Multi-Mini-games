#include "contextArena.h"
#include "utils/globals.h"
#include "setups/app.h"
#include "setups/game.h"

s64 initFonts(void) {
    u64 fontSize = 8;

    for (u64 fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        fonts[fontId] = LoadFontEx("assets/arial.ttf", fontSize, NULL, 0);
        if (!IsFontValid(fonts[fontId])) {
            log_warn("Font %zu (%d) wasn't proprely loaded", fontId, fontSize);
        }
        
        fontSize += 2;
    }

    return 0;
}

void freeFonts(void) {
    for (u64 fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}

s64 initApp(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    initGame();

    return 1;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    CloseWindow();
}
