/**
    @file main.c
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
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
    if (!polyBlast_initApp()) {
        log_fatal("Couldn't initialize correctly the app. Aborting");
        polyBlast_freeApp();
        return 1;
    }

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    f64 prevScore = polyBlast_mainGameState.scoring.score;

    bool windowShouldClose = false;

    while (!windowShouldClose) {
        Vector2 mouse = GetMousePosition();

        if (polyBlast_promptUpdate(&polyBlast_mainGameState, mouse)) {
            windowShouldClose = true;
        }

        if (polyBlast_currentPrompt == PROMPT_NONE && !polyBlast_mainGameState.gameOver) {
            bool allPlaced = true;
            for (u8 i = 0; i < 3; ++i) {
                polyBlast_handleShape(&polyBlast_mainGameState, &polyBlast_mainGameState.prefabManager.slots[i]);
    
                allPlaced &= polyBlast_mainGameState.prefabManager.slots[i].placed;
            }
    
            if (allPlaced) {
                polyBlast_adjustSizeWeights(&polyBlast_mainGameState, polyBlast_mainGameState.scoring.score - prevScore);
                // array_printContent("%.3f ", mainGameState.prefabManager.sizeWeights.runTimeWeights, MAX_SHAPE_SIZE);
                polyBlast_shuffleSlots(&polyBlast_mainGameState.prefabManager);
                // if (mainGameState.scoring.score > 0) {
                    polyBlast_placementSimulation(&polyBlast_mainGameState);
                // }
                prevScore = polyBlast_mainGameState.scoring.score;
            }

            if (polyBlast_testGameOver(polyBlast_mainGameState.board, polyBlast_mainGameState.prefabManager.slots)) {
                PlaySound(sound_gameOver);
                polyBlast_mainGameState.gameOver = true;

                if (polyBlast_mainGameState.gameOver && polyBlast_mainGameState.loadFilename != NULL && !polyBlast_mainGameState.hasBeenLost) {
                    polyBlast_mainGameState.hasBeenLost = true;
                    if (polyBlast_saveGameToFile(&polyBlast_mainGameState, polyBlast_mainGameState.loadFilename)) {
                        log_info("Marked loaded save as lost: %s", polyBlast_mainGameState.loadFilename);
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_R)) polyBlast_resetGame(&polyBlast_mainGameState);

        if (WindowShouldClose()) {
            if (polyBlast_currentPrompt == PROMPT_NONE && !polyBlast_mainGameState.gameOver) {
                polyBlast_currentPrompt = PROMPT_SAVE_QUIT;
            } else {
                windowShouldClose = true;
            }
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            if (polyBlast_currentPrompt == PROMPT_NONE) {
                polyBlast_drawUI(&polyBlast_mainGameState);
            }

            polyBlast_promptDraw();
        } EndDrawing();
    }

    polyBlast_freeApp();

    return 0;
}