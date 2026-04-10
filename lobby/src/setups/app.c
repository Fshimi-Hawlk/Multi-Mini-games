#include "setups/app.h"
#include "setups/audio.h"

#include "utils/globals.h"

void lobby_initFonts(void) {
    f32 fontSizes[__fontSizeCount] = {12, 24, 32, 48, 64, 96, 128};

    for (u8 i = 0; i < __fontSizeCount; ++i) {
        lobby_fonts[i] = LoadFontEx(FONTS_PATH "Noto/static/NotoSansMono-Bold.ttf", fontSizes[i], NULL, 0);
        if (!IsFontValid(lobby_fonts[i])) {
            log_warn("Font %s of size %f couldn't load properly", FONTS_PATH "Noto/static/NotoSansMono-Bold.ttf", fontSizes[i]);
        }
    }
}

void lobby_freeFonts(void) {
    for (u8 i = 0; i < __fontSizeCount; ++i) {
        if (!IsFontValid(lobby_fonts[i])) continue;
        UnloadFont(lobby_fonts[i]);
    }
}

void lobby_initApp(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    SetTargetFPS(60);

    lobby_initFonts();

    lobby_initAudio();

    paramsMenu_init(&paramsMenu);
}

void lobby_freeApp(void) {
    lobby_freeAudio();

    lobby_freeFonts();

    CloseWindow();
}