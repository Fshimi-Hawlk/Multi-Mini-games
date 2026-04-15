/**
    @file main.c
    @author Maxime CHAUVEAU
    @date February 2026
    @date 2026-04-07
    @brief Suika game entry point for standalone mode
*/
#include "core/gameAPI.h"
#include "core/game.h"
#include "utils/types.h"
#include "utils/configs.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils/audio.h"

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT, "Suika Game - Multi Mini-Games");
    SetTargetFPS(60);

    InitAudioDevice();
    suika_initAudio();

    SuikaGame_St* game = NULL;
    Error_Et error = suika_initGame(&game);

    if (error != OK)
    {
        log_error("Failed to initialize Suika game: error %d", error);
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose())
    {
        suika_gameLoop(game);

        if (!game->base.running)
        {
            break;
        }
    }

    CloseAudioDevice();
    suika_freeAudio();

    suika_freeGame(&game);
    CloseWindow();

    return 0;
}

#define PARAMS_MENU_IMPLEMENTATION
#include "paramsMenu.h"