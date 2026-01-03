#include "setups/game.h"
#include "setups/shape.h"
#include "utils/globals.h"

void initGame(void) {
    game.board.width = game.board.height = 8;

    f32Vector2 boardPos = {
        .x = WINDOW_WIDTH / 2.0f,
        .y = WINDOW_HEIGHT / 3.0f
    };

    f32Vector2 boardPxSize = {
        .x = BLOCK_PX_SIZE * game.board.width,
        .y = BLOCK_PX_SIZE * game.board.height,
    };

    game.board.pos = (f32Vector2) {
        .x = boardPos.x - boardPxSize.x / 2.0f ,
        .y = boardPos.y - boardPxSize.y / 2.0f 
    };

    da_reserve(&prefabsBag, 200);
    initPrefabsAndVariants(&prefabsBag);

    f32 offset = BLOCK_PX_SIZE * 8.0f;
    u8 prefabPerRow = (WINDOW_WIDTH - offset) / offset;
    u8 prefabPerCol = (WINDOW_HEIGHT - offset) / offset;

    log_info("%u, %u", prefabPerRow, prefabPerCol);

    shapeBag = calloc(prefabsBag.count, sizeof(*shapeBag));
    for (u32 i = 0; i < prefabsBag.count; ++i) {
        shapeBag[i] = (ActivePrefab_St) {
            .prefab = &prefabsBag.items[i],
            .colorIndex = i % (_blockColorCount - 1),
            .center = {
                .x = offset * (1 + i % prefabPerRow),
                .y = offset * (1 + round((f32) i / prefabPerRow))
            }
        };
    }

    for (u8 i = 0; i < 3; ++i) {
        ActivePrefab_St* const shape = &game.slots[i];
        shape->prefab = &prefabsBag.items[rand() % prefabsBag.count];
        shape->center = defaultPositions[i];
        shape->colorIndex = rand() % _blockColorCount;
        shape->placed = false;
    }
}