#include "utils/globals.h"
#include "ui/game.h"
#include "core/game/shape.h"
#include "setups/app.h"

int main(void) {
    initApp();

    log_info("%u", prefabsBag.count);

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