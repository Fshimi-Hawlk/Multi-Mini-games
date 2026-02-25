/**
 * @file main.c
 * @brief Suika game entry point for standalone mode
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "raylib.h"
#include "suikaAPI.h"
#include "utils/types.h"
#include "utils/configs.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(SUIKA_SCREEN_WIDTH, SUIKA_SCREEN_HEIGHT, "Suika Game - Multi Mini-Games");
    SetTargetFPS(60);

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

    suika_freeGame(&game);
    CloseWindow();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
