/**
 * @file game.h (setups)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Full game state initialization implementation.
 */

#include "setups/game.h"
#include "setups/shape.h"

#include "core/game/game.h"
#include "core/game/shape.h"

#include "utils/globals.h"
#include "utils/utils.h"

void initGame(void) {
    switch (game.sceneState) {
        case SCENE_STATE_GAME: {
            game.board.width = game.board.height = 8;
            game.board.rowsToClear = context_alloc(game.board.height * sizeof(bool8));
            game.board.columnsToClear = context_alloc(game.board.width * sizeof(bool8));
        
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
        
            buildScoreText();
        
            da_reserve(&prefabsBag, 200);
            initPrefabsAndVariants(&prefabsBag);
        
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                u8 size_idx = prefabsBag.items[i].blockCount - 1;
                da_append(&bags[size_idx], i);
            }
        
            for (u8 s = 0; s < MAX_BLOCK_PER_SHAPE; ++s) {
                PrefabIndexBag_St* bag = &bags[s];
                if (bag->count == 0) continue;
                da_shuffle(bag);
            }

            shuffleSlots(&game);
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            f32 offset = BLOCK_PX_SIZE * 8.0f;
            u8 prefabPerRow = (WINDOW_WIDTH - offset) / offset;
            // u8 prefabPerCol = (WINDOW_HEIGHT - offset) / offset;
        
            // log_info("%u, %u", prefabPerRow, prefabPerCol);
        
            shapeBag = context_alloc(prefabsBag.count * sizeof(*shapeBag));
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                shapeBag[i] = (ActivePrefab_St) {
                    .prefab = &prefabsBag.items[i],
                    .colorIndex = i % (_blockColorCount - 1),
                    .center = {
                        .x = offset * (1 + i % prefabPerRow),
                        .y = offset * (1 + floor((f32) i / prefabPerRow))
                    }
                };
            }
        } break;

        default: UNREACHABLE("GameSceneState_St");
    }
}