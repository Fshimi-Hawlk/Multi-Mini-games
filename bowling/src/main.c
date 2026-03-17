/**
 * @file main.c
 * @author Maxime CHAUVEAU
 * @date February 2026
 * @brief Main entry point for the Bowling mini-game.
 */

#include <stdlib.h>
#include <time.h>
#include "bowlingAPI.h"
#include "utils/types.h"
#include "logger.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bowling - Multi Mini-Games");
    SetTargetFPS(60);

    BowlingGame_St* game = NULL;
    bowling_initGame(&game);

    while (!WindowShouldClose()) {
        bowling_gameLoop(game);
    }

    bowling_freeGame(&game);
    CloseWindow();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"