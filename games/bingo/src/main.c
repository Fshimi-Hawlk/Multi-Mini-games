/**
    @file main.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-04-14
    @brief Entry point for the Bingo game application.
*/
#include "setups/app.h"

#include "utils/common.h"

#include "bingoAPI.h"
#include "logger.h"

/**
    @brief Entry point for the Bingo game.
    @param[in]     void
    @return                    Exit code (0 for success, 1 for failure)
*/
s32 main(void) {
    if (!initApp()) {
        log_fatal("Application initialization failed. Aborting.");
        freeApp();
        return 1;
    }

    BingoGame_St* game = NULL;

    bingo_initGame(&game);

    while (!WindowShouldClose()) {
        bingo_gameLoop(game);
    }

    bingo_freeGame(&game);

    freeApp();
    return 0;
}

// Required when contextArena.h is used for custom allocators
// #define CONTEXT_ARENA_IMPLEMENTATION
// #include "contextArena.h"