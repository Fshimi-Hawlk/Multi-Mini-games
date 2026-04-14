/**
    @file app.c
    @author Kimi BERGE
    @date 2026-03-02
    @date 2026-04-14
    @brief Application setup and teardown for the Bingo game.
*/
#include "setups/app.h"
#include "utils/globals.h"

/**
    @brief Loads a specific font size into the bingo_fonts array.

    @param[in]     fontId       The index in the bingo_fonts array.
    @param[in]     fontSize     The size of the font to load.
    @return                     True if the font was loaded successfully, false otherwise.
*/
static bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    bingo_fonts[fontId] = LoadFontEx(ASSET_PATH "fonts/Noto/static/NotoSansMono-Bold.ttf", fontSize, NULL, 0);
    if (!IsFontValid(bingo_fonts[fontId])) {
        log_warn("Font %zu (%f) wasn't proprely loaded", fontId, fontSize);
        return false;
    }
    return true;
}

/**
    @brief Initializes and loads all application fonts.

    @return                     True if all fonts were loaded successfully, false otherwise.
*/
bool bingo_initFonts(void) {
    u64 fontSize = 4;
    bool allFontLoaded = true;
    for (u64 fontId = 0; allFontLoaded && fontId < __fontSizeCount; fontId++) {
        allFontLoaded = loadFontIdForSize(fontId, fontSize);
        fontSize += 2;
    }
    return allFontLoaded;
}

/**
    @brief Frees all loaded fonts from memory.
*/
void bingo_freeFonts(void) {
    for (u64 fontId = 0; fontId < __fontSizeCount; fontId++) {
        UnloadFont(bingo_fonts[fontId]);
    }
}

/**
    @brief Initializes the application: window, logger, and fonts.

    @return                     True if initialization succeeded, false otherwise.
*/
bool bingo_initApp(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

#ifdef LOGGER_H
    initLogger();
#endif

    if (!bingo_initFonts()) {
        log_warn("Couldn't initialize every bingo_fonts");
    };

    return true;
}

/**
    @brief Frees application-wide resources and closes the window.
*/
void bingo_freeApp(void) {
    bingo_freeFonts();
    CloseWindow();
}
