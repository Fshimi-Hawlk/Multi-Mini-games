/**
    @file main.c
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Program entry point and main loop.
*/

#include "core/shape.h"
#include "core/placement.h"
#include "core/game.h"

#include "setups/save.h"
#include "ui/game.h"

#include "setups/app.h"

#include "utils/common.h"
#include "utils/globals.h"

int main(void) {
    if (!initApp()) {
        log_fatal("Couldn't initialize correctly the app. Aborting");
        freeApp();
        return 1;
    }

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    f64 prevScore = mainGameState.scoring.score; // TODO: Useful when loading a mainGameState from save file

    while (!WindowShouldClose()) {
        if (!mainGameState.gameOver) {
            bool allPlaced = true;
            for (u8 i = 0; i < 3; ++i) {
                handleShape(&mainGameState, &mainGameState.prefabManager.slots[i]);
    
                allPlaced &= mainGameState.prefabManager.slots[i].placed;
            }
    
            if (allPlaced) {
                adjustSizeWeights(&mainGameState, mainGameState.scoring.score - prevScore);
                // array_printContent("%.3f ", mainGameState.prefabManager.sizeWeights.runTimeWeights, MAX_SHAPE_SIZE);
                shuffleSlots(&mainGameState.prefabManager);
                // if (mainGameState.scoring.score > 0) {
                    placementSimulation(&mainGameState);
                // }
                prevScore = mainGameState.scoring.score;
            }
    
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) {
                u64 newStateLoadingBufferSize = getSerializedGameStateSize(&mainGameState);
                if (newStateLoadingBufferSize != stateLoadingBufferSize) {
                    stateLoadingBufferSize = newStateLoadingBufferSize;
                    if (stateLoadingBuffer != NULL) free(stateLoadingBuffer);
                    stateLoadingBuffer = malloc(stateLoadingBufferSize);
                }

                if (stateLoadingBuffer == NULL) {
                    log_error("Couldn't allocate stateLoadingBuffer");
                }

                s64 diff = serializeGameState(&mainGameState, stateLoadingBuffer, stateLoadingBufferSize);
                if (diff != 0) {
                    log_warn("The new computed buffer size is incorrect by %zi", diff);
                }
            } else if (IsKeyPressed(KEY_S)) {
                shuffleSlots(&mainGameState.prefabManager);
            }
        }
        
        if (IsKeyPressed(KEY_U) && stateLoadingBuffer != NULL) {
            if (!deserializeGameState(&mainGameState, stateLoadingBuffer, stateLoadingBufferSize, false)) {
                log_warn("Couldn't deserialized last save");
            }
            
            buildScoreRelatedTexts(&mainGameState.scoring);
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            drawUI(&mainGameState);
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