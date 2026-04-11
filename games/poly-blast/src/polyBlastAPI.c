/**
    @file polyblast.c
    @author Fshimi-Hawlk
    @date 2026-02-07
    @date 2026-04-09
    @brief Implementation of the PolyBlast mini-game.
*/

#include "core/shape.h"
#include "core/placement.h"
#include "core/game.h"

#include "setups/save.h"
#include "setups/game.h"
#include "setups/audio.h"
#include "setups/shape.h"

#include "ui/game.h"
#include "ui/prompt.h"

#include "setups/app.h"

#include "utils/globals.h"

#include "polyBlastAPI.h"

#include "sharedUtils/debug.h"

#include "APIs/generalAPI.h"

#include "paramsMenu.h"

// Global params menu state for polyblast
static ParamsMenu_St polyblastParamsMenu = {0};

struct PolyBlastGame_St {
    BaseGame_St base;               // Must be first - allows safe cast to BaseGame_St*

    GameState_St state;
    f32 prevScore;
};

/* -----------------------------------
      Lifecycle implementation
   ----------------------------------- */

Error_Et polyBlast_freeGameWrapper(void* game) {
    return polyBlast_freeGame((PolyBlastGame_St**) game);
}

Error_Et polyBlast_initGame__full(PolyBlastGame_St** game, PolyBlastConfigs_St configs) {
    UNUSED(configs);

    polyBlast_initAudio();

    *game = malloc(sizeof(PolyBlastGame_St));
    if (*game == NULL) {
        log_error("Failed to allocate PolyBlastGame_St");
        return ERROR_ALLOC;
    }

    PolyBlastGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    gameRef->base.freeGame  = polyBlast_freeGameWrapper;
    gameRef->base.running   = true;

    polyBlast_initPrefabsAndVariants(&polyBlast_prefabsBag, polyBlast_prefabVariant);
    polyBlast_initPrefabManager(&gameRef->state.prefabManager);
    
    gameRef->state.board.width = gameRef->state.board.height = 8;
    polyBlast_initBoard(&gameRef->state.board);

    // Initialize params menu (settings button)
    paramsMenu_init(&polyblastParamsMenu);

    return OK;
}

Error_Et polyBlast_gameLoop(PolyBlastGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return OK;
    }

    // Update params menu (settings button clicks)
    paramsMenu_update(&polyblastParamsMenu);

    Vector2 mouse = GetMousePosition();

    if (polyBlast_promptUpdate(&game->state, mouse)) {
        game->base.running = true;
    }

    if (currentPrompt == PROMPT_NONE && !game->state.gameOver) {
        bool allPlaced = true;
        for (u8 i = 0; i < 3; ++i) {
            polyBlast_handleShape(&game->state, &game->state.prefabManager.slots[i]);

            allPlaced &= game->state.prefabManager.slots[i].placed;
        }

        if (allPlaced) {
            polyBlast_adjustSizeWeights(&game->state, game->state.scoring.score - game->prevScore);
            // array_printContent("%.3f ", game->state.prefabManager.sizeWeights.runTimeWeights, MAX_SHAPE_SIZE);
            polyBlast_shuffleSlots(&game->state.prefabManager);
            // if (game->state.scoring.score > 0) {
                polyBlast_placementSimulation(&game->state);
            // }
            game->prevScore = game->state.scoring.score;
        }

        if (polyBlast_testGameOver(game->state.board, game->state.prefabManager.slots)) {
            PlaySound(sound_gameOver);
            game->state.gameOver = true;

            if (game->state.gameOver && game->state.loadFilename != NULL && !game->state.hasBeenLost) {
                game->state.hasBeenLost = true;
                if (polyBlast_saveGameToFile(&game->state, game->state.loadFilename)) {
                    log_info("Marked loaded save as lost: %s", game->state.loadFilename);
                }
            }
        }
    }

    if (IsKeyPressed(KEY_R)) polyBlast_resetGame(&game->state);

    if (WindowShouldClose()) {
        if (currentPrompt == PROMPT_NONE && !game->state.gameOver) {
            currentPrompt = PROMPT_SAVE_QUIT;
        } else {
            game->base.running = true;
        }
    }

    BeginDrawing(); {
        ClearBackground(APP_BACKGROUND_COLOR);

        if (currentPrompt == PROMPT_NONE) {
            polyBlast_drawUI(&game->state);
        }

        polyBlast_promptDraw();
    } EndDrawing();

    return OK;
}

Error_Et polyBlast_freeGame(PolyBlastGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;

    // Cleanup params menu
    paramsMenu_free(&polyblastParamsMenu);

    arena_free(&tempArena);

    polyBlast_freeFonts();

    polyBlast_freeAudio();

    free(*game);
    *game = NULL;

    return OK;
}
