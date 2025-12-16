#include "setups/app.h"
#include "utils/globals.h"

s64_t initFonts(void) {
    u64_t fontSize = 8;

    for (u64_t fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        fonts[fontId] = LoadFontEx("assets/arial.ttf", fontSize, NULL, 0);
        if (!IsFontValid(fonts[fontId])) {
            log_warn("Font %zu (%d) wasn't proprely loaded", fontId, fontSize);
        }
        
        fontSize += 2;
    }

    return 0;
}

void freeFonts(void) {
    for (u64_t fontId = 0; fontId < MAX_FONT_COUNT; fontId++) {
        UnloadFont(fonts[fontId]);
    }
}