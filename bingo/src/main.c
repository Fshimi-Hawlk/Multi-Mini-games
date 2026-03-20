/**
    @file main.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-16
    @brief One clear sentence that tells what this file is actually for.
    
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
            - Initial implementation
            - Refactored into structured state and helper functions

    @note Uses Raylib for rendering and input.
          Numbers 0-99 placed randomly without column restrictions.
          Calls use B/I/N/G/O format but do not enforce column ranges.
*/

#include "setups/app.h"

#include "utils/common.h"

#include "bingoAPI.h"
#include "logger.h"

// ────────────────────────────────────────────────
// Entry point
// ────────────────────────────────────────────────

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

#define LOGGER_IMPLEMENTATION
#include "logger.h"

// Required when contextArena.h is used for custom allocators
// #define CONTEXT_ARENA_IMPLEMENTATION
// #include "contextArena.h"