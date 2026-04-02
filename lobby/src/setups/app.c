/**
    @file app.c
    @author Fshimi Hawlk
    @date 2026-03-02
    @date 2026-03-16
    @brief Application setup and teardown.
*/

#include "setups/app.h"
#include "APIs/generalAPI.h"
#include "setups/game.h"

#include "utils/common.h"
#include "utils/globals.h"

#include "systemSettings.h"

bool loadFontIdForSize(u64 fontId, f32 fontSize) {
    lobby_fonts[fontId] = LoadFontEx(FONT_PATH "Noto/static/NotoSansMono-Bold.ttf", fontSize, NULL, 0);
        if (!IsFontValid(lobby_fonts[fontId])) {
            log_warn("Font %zu (%f) wasn't proprely loaded", fontId, fontSize);
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
        UnloadFont(lobby_fonts[fontId]);
    }
}

Error_Et initApp(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);

    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.resizable = true;
    systemSettings.video.title = "Lobby";

    // ── Initialization ───────────────────────────────────────────────────────
    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    SetWindowPosition(100, 50);

#ifdef LOGGER_H
    init_logger();
#endif

    if (!initFonts()) {
        log_warn("Couldn't initialize every lobby_fonts");
    }

    if (gameInit() != OK) {
        log_error("Couldn't load game data");
        return ERROR_ASSET_LOAD;
    }

    return OK;
}

void freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    freeFonts();

    CloseWindow();
}
