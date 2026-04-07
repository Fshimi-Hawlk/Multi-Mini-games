/**
 * @file main.c
 * @author Maxime CHAUVEAU
 * @date February 2026
 * @brief Main entry point for the Bowling mini-game.
 */

#include <stdlib.h>
#include <time.h>
#include "core/game.h"
#include "utils/types.h"
#include "logger.h"
#include "utils/audio.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bowling - Multi Mini-Games");
    SetTargetFPS(60);

    InitAudioDevice();
    bowling_initAudio();

    BowlingGame_St* game = NULL;
    bowling_initGame(&game);

    while (!WindowShouldClose()) {
        bowling_gameLoop(game);
    }

    bowling_freeGame(&game);

    bowling_freeAudio();
    CloseAudioDevice();

    CloseWindow();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"