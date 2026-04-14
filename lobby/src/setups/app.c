/**
    @file app.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief app.c implementation/header file
*/
#include "setups/app.h"
#include "setups/audio.h"
#include "setups/texture.h"

#include "utils/globals.h"

void lobby_initFonts(void) {
    f32 fontSizes[__fontSizeCount] = {16, 24, 32, 48, 64, 96, 128};

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

Error_Et lobby_initApp(void) {
    Error_Et error = OK;
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    InitAudioDevice();
    SetTargetFPS(60);

    lobby_initFonts();
    lobby_initAudio();

    error = lobby_initTextures(lobby_game.playerVisuals.textures);
    if (error != OK) {
        log_warn("[Lobby] Textures didn't load properly.");
    }

    paramsMenu_init(&paramsMenu);

    return error;
}

void lobby_freeApp(void) {
    arena_free(&globalArena);
    arena_free(&tempArena);

    lobby_freeAudio();
    lobby_freeFonts();

    lobby_freeTextures(lobby_game.playerVisuals.textures);

    CloseAudioDevice();

    CloseWindow();
}
