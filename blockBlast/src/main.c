#include "utils/globals.h"

#include "setups/app.h"

#include "core/game/shape.h"

#include "ui/game.h"

int main(void) {
    initApp();

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    // log_info("%u", prefabsBag.count);

    while (!WindowShouldClose()) {
        bool8 allPlaced = true;
        for (u8 i = 0; i < 3; ++i) {
            handleShape(&game.slots[i]);

            allPlaced &= game.slots[i].placed;
        }

        if (allPlaced) {
            shuffleSlots(&game);
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