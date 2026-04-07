/**
    @file main.c
    @author Fshimi Hawlk
    @date 2026-01-07
    @date 2026-04-07
    @brief Program entry point and main loop.
*/

#include "core/shape.h"
#include "core/placement.h"
#include "core/game.h"

#include "setups/save.h"
#include "setups/game.h"

#include "ui/game.h"
#include "ui/prompt.h"

#include "setups/app.h"

#include "utils/globals.h"

int main(void) {
    if (!initApp()) {
        log_fatal("Couldn't initialize correctly the app. Aborting");
        freeApp();
        return 1;
    }

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    f64 prevScore = mainGameState.scoring.score;

    bool windowShouldClose = false;

    while (!windowShouldClose) {
        Vector2 mouse = GetMousePosition();

        if (promptUpdate(&mainGameState, mouse)) {
            windowShouldClose = true;
        }

        if (currentPrompt == PROMPT_NONE && !mainGameState.gameOver) {
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

            if (testGameOver(mainGameState.board, mainGameState.prefabManager.slots)) {
                PlaySound(sound_gameOver);
                mainGameState.gameOver = true;

                if (mainGameState.gameOver && mainGameState.loadFilename != NULL && !mainGameState.hasBeenLost) {
                    mainGameState.hasBeenLost = true;
                    if (saveGameToFile(&mainGameState, mainGameState.loadFilename)) {
                        log_info("Marked loaded save as lost: %s", mainGameState.loadFilename);
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_R)) resetGame(&mainGameState);

        if (WindowShouldClose()) {
            if (currentPrompt == PROMPT_NONE && !mainGameState.gameOver) {
                currentPrompt = PROMPT_SAVE_QUIT;
            } else {
                windowShouldClose = true;
            }
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            if (currentPrompt == PROMPT_NONE) {
                drawUI(&mainGameState);
            }

            promptDraw();
        } EndDrawing();
    }

    freeApp();

    return 0;
}