#include "utils/globals.h"
#include "ui/game.h"
#include "core/game/shape.h"
#include "setups/app.h"

int main(void) {
    initApp();

    log_info("%u", prefabsBag.count);

    while (!WindowShouldClose()) {
        for (u8 i = 0; i < 3; ++i) {
            handleShape(&game.slots[i]);
        }

        if (IsKeyPressed(KEY_S)) {
            for (u8 i = 0; i < 3; ++i) {
                ActivePrefab_St* const shape = &game.slots[i];
                shape->prefab = &prefabsBag.items[rand() % prefabsBag.count];
                shape->center = defaultPositions[i];
                shape->colorIndex = rand() % _blockColorCount;
                shape->placed = false;
            }
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

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"