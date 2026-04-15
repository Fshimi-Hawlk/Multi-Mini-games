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
#include "setups/game.h"

#include "utils/globals.h"

static PolyBlastGame_St polyBlast_game = {0};

int main(void) {
    if (!polyBlast_initApp()) {
        log_fatal("Couldn't initialize correctly the app. Aborting");
        polyBlast_freeApp();
        return 1;
    }

    polyBlast_initGame(&polyBlast_game);

    // game.sceneState = SCENE_STATE_ALL_PREFABS;

    f64 prevScore = polyBlast_game.scoring.score;

    bool windowShouldClose = false;

    while (!windowShouldClose) {
        Vector2 mouse = GetMousePosition();

        if (polyBlast_promptUpdate(&polyBlast_game, mouse)) {
            windowShouldClose = true;
        }

        if (polyBlast_currentPrompt == PROMPT_NONE && !polyBlast_game.gameOver) {
            bool allPlaced = true;
            for (u8 i = 0; i < 3; ++i) {
                polyBlast_handleShape(&polyBlast_game, &polyBlast_game.prefabManager.slots[i]);
    
                allPlaced &= polyBlast_game.prefabManager.slots[i].placed;
            }
    
            if (allPlaced) {
                polyBlast_adjustSizeWeights(&polyBlast_game, polyBlast_game.scoring.score - prevScore);
                // array_printContent("%.3f ", mainGameState.prefabManager.sizeWeights.runTimeWeights, MAX_SHAPE_SIZE);
                polyBlast_shuffleSlots(&polyBlast_game.prefabManager);
                // if (mainGameState.scoring.score > 0) {
                    polyBlast_placementSimulation(&polyBlast_game);
                // }
                prevScore = polyBlast_game.scoring.score;
            }

            if (polyBlast_testGameOver(polyBlast_game.board, polyBlast_game.prefabManager.slots)) {
                PlaySound(sound_gameOver);
                polyBlast_game.gameOver = true;

                if (polyBlast_game.gameOver && polyBlast_game.loadFilename != NULL && !polyBlast_game.hasBeenLost) {
                    polyBlast_game.hasBeenLost = true;
                    if (polyBlast_saveGameToFile(&polyBlast_game, polyBlast_game.loadFilename)) {
                        log_info("Marked loaded save as lost: %s", polyBlast_game.loadFilename);
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_R)) polyBlast_resetGame(&polyBlast_game);

        if (WindowShouldClose()) {
            if (polyBlast_currentPrompt == PROMPT_NONE && !polyBlast_game.gameOver) {
                polyBlast_currentPrompt = PROMPT_SAVE_QUIT;
            } else {
                windowShouldClose = true;
            }
        }

        BeginDrawing(); {
            ClearBackground(APP_BACKGROUND_COLOR);

            if (polyBlast_currentPrompt == PROMPT_NONE) {
                polyBlast_drawUI(&polyBlast_game);
            }

            polyBlast_promptDraw();
        } EndDrawing();
    }

    polyBlast_freeApp();

    return 0;
}