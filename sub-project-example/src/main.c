/**
    @file main.c
    @author Fshimi-Hawlk
    @date 2026-01-25              // Creation date - never touch this one again
    @date 2026-02-23              // Last time someone meaningfully changed this file (update only when needed)
    @brief One clear sentence that tells what this file is actually for.
    
    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "utils/common.h"
#include "setups/app.h"

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
*/
int main(void) {
    // ── Initialization ───────────────────────────────────────────────────────
    if (!initApp()) {
        log_fatal("Application initialization failed. Aborting.");
        freeApp();
        return 1;
    }

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {

        // Game logic goes here (update, input, simulation)
        // ...

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            // Game rendering goes here
            // ...
            
        } EndDrawing();
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────
    freeApp();

    return 0;
}

// Required when logger.h is used
#define LOGGER_IMPLEMENTATION
#include "logger.h"

// Required when contextArena.h is used for custom allocators
#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

// Required when custom rand.h implementation is used (not default)
#if defined RAND_H
#define RAND_IMPLEMENTATION
#include "rand.h"
#endif