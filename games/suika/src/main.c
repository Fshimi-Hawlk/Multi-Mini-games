/**
    @file main.c
    @author Maxime CHAUVEAU
    @date February 2026
    @date 2026-04-07
    @brief Suika game entry point for standalone mode
*/

#include "core/game.h"
#include "core/audio.h"

#include "raylib.h"
#include "utils/types.h"
#include "utils/configs.h"

#include <stdlib.h>
#include <time.h>


int main(void) {
    srand(time(NULL));

    InitWindow(SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT, "Suika");
    SetTargetFPS(60);

    InitAudioDevice();
    suika_initAudio();

    suika_initAudio();

    SuikaGame_St game = {0};
    suika_initGame(&game);
    suika_loadAssets(&game);

    while (!WindowShouldClose()) {
        suika_updateGame(&game, GetFrameTime());

        BeginDrawing(); {
            suika_drawGame(&game);
        } EndDrawing();

    }

    suika_unloadAssets(&game);
    suika_freeAudio();
    CloseAudioDevice();

    CloseWindow();

    return 0;
}