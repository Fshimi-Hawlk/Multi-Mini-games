/**
    @file game.h (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Full game state initialization implementation.
*/

#include "setups/game.h"
#include "setups/save.h"
#include "setups/shape.h"

#include "core/game.h"
#include "core/shape.h"
#include "core/placement.h"

#include "utils/globals.h"
#include "utils/utils.h"

/**
    @brief Initializes size-based runTimeWeights for random prefab selection.

    Sets baseWeights to fixed probabilities, copies to runTimeWeights for runtime adjustment.
    Larger shapes might have lower runTimeWeights to control difficulty.

    @param game Pointer to the current game state.
*/
static void initSizeWeights(PrefabManager_St* const manager) {
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
        manager->sizeWeights.runTimeWeights[i] = manager->sizeWeights.baseWeights[i] = baseWeights[i];
    }
}

bool initBoard(Board_St* const board) {
    if (board == NULL) {
        log_warn("Received NULL board");
        return false;
    }

    f32Vector2 boardPos = {
        .x = WINDOW_WIDTH / 2.0f,
        .y = WINDOW_HEIGHT / 3.0f
    };

    f32Vector2 boardPxSize = {
        .x = BLOCK_PX_SIZE * board->width,
        .y = BLOCK_PX_SIZE * board->height,
    };

    board->pos = (f32Vector2) {
        .x = boardPos.x - boardPxSize.x / 2.0f ,
        .y = boardPos.y - boardPxSize.y / 2.0f
    };

    board->rowsToClear = context_alloc(board->height * sizeof(bool));
    board->columnsToClear = context_alloc(board->width * sizeof(bool));

    return true;
}

bool initPrefabManager(PrefabManager_St* const manager) {
    if (manager == NULL) {
        log_warn("Received NULL manager");
        return false;
    }

    initSizeWeights(manager);

    for (u32 i = 0; i < prefabsBag.count; ++i) {
        u8 size_idx = prefabsBag.items[i].blockCount - 1;
        da_append(&manager->bags[size_idx], i);
    }

    for (u8 s = 0; s < MAX_SHAPE_SIZE; ++s) {
        PrefabIndexBagVec_St* bag = &manager->bags[s];
        if (bag->count == 0) continue;
        da_shuffleXor(bag, rand);
    }

    shuffleSlots(manager);

    return true;
}

bool initGame(GameState_St* const game, bool fromLoad) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            if (fromLoad) {
                initPrefabManager(&game->prefabManager);
                
                if (!deserializeGameState(game, stateLoadingBuffer, stateLoadingBufferSize, true)) {
                    log_warn("Can't load that save file");
                }

                initBoard(&game->board);
                buildScoreRelatedTexts(&game->scoring);
            } else {
                initPrefabsAndVariants(&prefabsBag, prefabVariant);
                initPrefabManager(&game->prefabManager);

                game->board.width = game->board.height = 8;
                initBoard(&game->board);
                buildScoreRelatedTexts(&game->scoring);
                
                placementSimulation(game);
            }

            return true;
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            f32 offset = BLOCK_PX_SIZE * 8.0f;
            u8 prefabPerRow = (WINDOW_WIDTH - offset) / offset;
            // u8 prefabPerCol = (WINDOW_HEIGHT - offset) / offset;

            // log_info("%u, %u", prefabPerRow, prefabPerCol);

            shapeBag = context_alloc(prefabsBag.count * sizeof(*shapeBag));
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                shapeBag[i] = (Shape_St) {
                    .prefab = &prefabsBag.items[i],
                    .colorIndex = i % (_blockColorCount - 1),
                    .center = {
                        .x = offset * (1 + i % prefabPerRow),
                        .y = offset * (1 + floor((f32) i / prefabPerRow))
                    }
                };
            }

            return true;
        } break;

        default: UNREACHABLE("GameSceneState_St");
    }
}