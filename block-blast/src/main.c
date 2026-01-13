/**
 * @file main.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Program entry point and main loop.
 */

#include "core/game/shape.h"
#include "core/game/game.h"
#include "setups/app.h"
#include "ui/game.h"

#include "utils/globals.h"
#include "utils/utils.h"

int main(void) {
    if (!initApp()) {
        log_fatal("Couldn't initialize correctly the app. Aborting");
        freeApp();
        return 1;
    }

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    // log_info("%u", prefabsBag.count);

    f64 prevScore = game.score;

    while (!WindowShouldClose()) {
        bool8 allPlaced = true;
        for (u8 i = 0; i < 3; ++i) {
            handleShape(&game.slots[i]);

            allPlaced &= game.slots[i].placed;
        }

        if (allPlaced) {
            adjustSizeWeights(&game, game.score - prevScore);
            for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
                printf("%.3f ", game.sizeWeights.weights[i]);
            }
            nl
            shuffleSlots(&game);
            prevScore = game.score;
        }

        if (IsKeyPressed(KEY_S)) {
            shuffleSlots(&game);
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            drawUI(&game);
        } EndDrawing();
    }

    freeApp();

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"