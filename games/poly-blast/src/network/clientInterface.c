#include "core/shape.h"
#include "core/placement.h"
#include "core/game.h"

#include "setups/save.h"
#include "setups/app.h"
#include "setups/game.h"
#include "setups/audio.h"
#include "setups/shape.h"

#include "systemSettings.h"
#include "ui/game.h"
#include "ui/prompt.h"

#include "setups/app.h"

#include "utils/globals.h"

#include "sharedUtils/debug.h"

#include "APIs/generalAPI.h"
#include "networkInterface.h"
#include "paramsMenu.h"

static ParamsMenu_St polyblast_paramsMenu = {0};
static PolyBlastGame_St polyBlast_game = {0};

void polyBlast_init(void) {
    polyBlast_initAudio();
    polyBlast_initFonts();

    systemSettings.video.width = WINDOW_WIDTH;
    systemSettings.video.height = WINDOW_HEIGHT;
    systemSettings.video.title = WINDOW_TITLE;
    applySystemSettings();

    memset(&polyBlast_game, 0, sizeof(polyBlast_game));

    polyBlast_initPrefabsAndVariants(&polyBlast_prefabsBag, polyBlast_prefabVariant);
    polyBlast_initPrefabManager(&polyBlast_game.prefabManager);
    
    polyBlast_game.board.width = polyBlast_game.board.height = 8;
    polyBlast_initBoard(&polyBlast_game.board);

    // Initialize params menu (settings button)
    paramsMenu_init(&polyblast_paramsMenu);
}

void polyBlast_update(f32 dt) {
    UNUSED(dt);

    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        extern void switchMinigame(u8 gameId);
        switchMinigame(MINI_GAME_ID_LOBBY);
        return;
    }

    // Update params menu (settings button clicks)
    paramsMenu_update(&polyblast_paramsMenu);

    Vector2 mouse = GetMousePosition();

    polyBlast_promptUpdate(&polyBlast_game, mouse);

    if (polyBlast_currentPrompt == PROMPT_NONE && !polyBlast_game.gameOver) {
        bool allPlaced = true;
        for (u8 i = 0; i < 3; ++i) {
            polyBlast_handleShape(&polyBlast_game, &polyBlast_game.prefabManager.slots[i]);

            allPlaced &= polyBlast_game.prefabManager.slots[i].placed;
        }

        if (allPlaced) {
            polyBlast_adjustSizeWeights(&polyBlast_game, polyBlast_game.scoring.score - polyBlast_game.scoring.prevScore);
            polyBlast_shuffleSlots(&polyBlast_game.prefabManager);
            polyBlast_placementSimulation(&polyBlast_game);
            polyBlast_game.scoring.prevScore = polyBlast_game.scoring.score;
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
        }
    }
}

void polyBlast_draw(void) {
    ClearBackground(APP_BACKGROUND_COLOR);

    if (polyBlast_currentPrompt == PROMPT_NONE) {
        polyBlast_drawUI(&polyBlast_game);
    }

    polyBlast_promptDraw();
}

void polyBlast_destroy(void) {
    // Cleanup params menu
    paramsMenu_free(&polyblast_paramsMenu);

    arena_free(&tempArena);

    polyBlast_freeFonts();
    polyBlast_freeAudio();
}

GameClientInterface_St polyBlast_clientInterface = {
    .id = MINI_GAME_ID_POLY_BLAST,
    .name = "Poly Blast",
    .init = polyBlast_init,
    .onData = NULL,
    .update = polyBlast_update,
    .draw = polyBlast_draw,
    .destroy = polyBlast_destroy
};