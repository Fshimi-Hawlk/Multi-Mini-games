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

/**
 * @brief Initializes size-based runTimeWeights for random prefab selection.
 *
 * Sets baseWeights to fixed probabilities, copies to runTimeWeights for runtime adjustment.
 * Larger shapes might have lower runTimeWeights to control difficulty.
 *
 * @param game Pointer to the current game state.
 */
static void initSizeWeights(GameState_St* const game) {
    f32 baseWeights[MAX_SHAPE_SIZE] = {
        [0] = 0.05f,    // size 1
        [1] = 0.20f,
        [2] = 0.25f,
        [3] = 0.25f,
        [4] = 0.10f,
        [5] = 0.05f,
        [6] = 0.00f,    // will never change, since no shape of size 7 exist, it's set by default to zero
        [7] = 0.00f,    // will never change, since no shape of size 8 exist, it's set by default to zero
        [8] = 0.10f     // size 9
    };

    for (u8 i = 0; i < MAX_SHAPE_SIZE; i++) {
        game->prefabManager.sizeWeights.runTimeWeights[i] = game->prefabManager.sizeWeights.baseWeights[i] = baseWeights[i];
    }
}

void initGame(GameState_St* const game) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            game->board.width = game->board.height = 8;
            game->board.rowsToClear = context_alloc(game->board.height * sizeof(bool));
            game->board.columnsToClear = context_alloc(game->board.width * sizeof(bool));

            initSizeWeights(game);

            f32Vector2 boardPos = {
                .x = WINDOW_WIDTH / 2.0f,
                .y = WINDOW_HEIGHT / 3.0f
            };

            f32Vector2 boardPxSize = {
                .x = BLOCK_PX_SIZE * game->board.width,
                .y = BLOCK_PX_SIZE * game->board.height,
            };

            game->board.pos = (f32Vector2) {
                .x = boardPos.x - boardPxSize.x / 2.0f ,
                .y = boardPos.y - boardPxSize.y / 2.0f
            };

            buildScoreRelatedTexts(game);

            da_reserve(&game->prefabManager.prefabsBag, 200);
            initPrefabsAndVariants(&game->prefabManager);

            for (u32 i = 0; i < game->prefabManager.prefabsBag.count; ++i) {
                u8 size_idx = game->prefabManager.prefabsBag.items[i].blockCount - 1;
                da_append(&game->prefabManager.bags[size_idx], i);
            }

            for (u8 s = 0; s < MAX_SHAPE_SIZE; ++s) {
                PrefabIndexBagVec_St* bag = &game->prefabManager.bags[s];
                if (bag->count == 0) continue;
                da_shuffle(bag);
            }

            shuffleSlots(&game->prefabManager);
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            f32 offset = BLOCK_PX_SIZE * 8.0f;
            u8 prefabPerRow = (WINDOW_WIDTH - offset) / offset;
            // u8 prefabPerCol = (WINDOW_HEIGHT - offset) / offset;

            // log_info("%u, %u", prefabPerRow, prefabPerCol);

            shapeBag = context_alloc(game->prefabManager.prefabsBag.count * sizeof(*shapeBag));
            for (u32 i = 0; i < game->prefabManager.prefabsBag.count; ++i) {
                shapeBag[i] = (ActivePrefab_St) {
                    .prefab = &game->prefabManager.prefabsBag.items[i],
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