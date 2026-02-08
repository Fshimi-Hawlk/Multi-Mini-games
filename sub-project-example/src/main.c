/**
 * @file main.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Minimal program entry point – application setup and main loop skeleton.
 *
 * This file serves as a starting template or fallback main.
 * In a full lobby-integrated build, most logic is moved to a dedicated main.c.
 */

#include "utils/common.h"           // Common macros, helpers
#include "setups/app.h"             // High-level app init/free

/**
 * @brief Program entry point.
 * @return 0 on clean exit, non-zero on early failure
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
        float dt = GetFrameTime();

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
#if !defined(_USE_DEFAULT_RAND)
// #define RAND_IMPLEMENTATION
// #include "rand.h"
#endif