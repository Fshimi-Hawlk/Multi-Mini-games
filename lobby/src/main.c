/**
 * @file main.c
 * @author LeandreB8 - Fshimi Hawlk
 * @date 2026-02-08
 * @brief Program entry point – lobby main loop and game scene manager.
 *
 * This file contains the top-level application loop.
 * It initializes the window and shared resources, runs the lobby,
 * and switches to individual games when triggered (e.g. collision with zone).
 *
 * Games are loaded on demand via their API (e.g. gameNameAPI.h) and run
 * in the same process/window. No separate executables are spawned.
 */

#include "core/game.h"

#include "setups/app.h"
#include "setups/game.h"

#include "ui/grass.h"
#include "ui/background.h"
#include "ui/ambiance.h"
#include "ui/game.h"
#include "ui/app.h"

#include "core/game.h"              // GameScene_Et, general game types
#include "ui/app.h"                 // UI helpers (skin menu, buttons, etc.)
#include "ui/game.h"                // Player drawing, platform logic

#include "utils/globals.h"          // Global constants (WINDOW_WIDTH, etc.)

#include "APIs/generalAPI.h"

#include "APIs/tetrisAPI.h"
#include "utils/userTypes.h"


void lobby_gameLoop(float dt) {
    gameTime += dt;

    if (gameTime > 1.45f) {
        updatePlayer(&game.player, platforms, platformCount, dt);
    }

    game.cam.target = game.player.position;

    toggleSkinMenu(&game);

    if (game.playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&game.player, &game);
    }

    updateGrass(&game.player, GetFrameTime(), gameTime, game.cam);
    updateAtmosphericEffects(dt, &game.player, game.cam);

    if (CheckCollisionCircleRec(game.player.position, game.player.radius, tetrisHitbox)) {
        if (!game.subGameManager.gameHitGracePeriodActive) {
            game.subGameManager.currentScene = GAME_SCENE_TETRIS;
            game.subGameManager.needGameInit = true;
            game.subGameManager.gameHitGracePeriodActive = true;
        }
    } else if (game.subGameManager.gameHitGracePeriodActive) {
        game.subGameManager.gameHitGracePeriodActive = false;
    }

    BeginDrawing(); {
        ClearBackground(RAYWHITE);

        BeginMode2D(game.cam); {
            drawStarryBackground(game.player.position, game.cam);

            drawTree();
            
            drawPlatforms(platforms, platformCount);
            drawPlayer(&game, &game.player);
            
            drawWorldBoundaries(&game.player);
            
            drawGrass(&game.player, game.cam);
            drawAtmosphericEffects();
        } EndMode2D();

        drawScreenEffects(&game.player);

        DrawText("Multi-Mini-Games", WINDOW_WIDTH / 2.0f - MeasureText("Multi-Mini-Games", 20) / 2.0f, 20, 20, PURPLE);
        drawSkinButton();
        if (game.playerVisuals.isTextureMenuOpen) {
            drawMenuTextures(&game);
        }

    } EndDrawing();
}

// ─────────────────────────────────────────────────────────────────────────────
// Program entry point
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    lobby_initApp();
    lobby_initGame();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        switch (game.subGameManager.currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(dt);
            } break;

            case GAME_SCENE_TETRIS: {
                BaseGame_St* base = game.subGameManager.miniGames[GAME_SCENE_TETRIS];
                TetrisGame_St** tetrisGame = (TetrisGame_St**) &game.subGameManager.miniGames[GAME_SCENE_TETRIS];
                if (game.subGameManager.needGameInit) {
                    game.subGameManager.miniGames[GAME_SCENE_TETRIS] = (BaseGame_St*) tetris_initGame();
                    game.subGameManager.needGameInit = false;
                }

                tetris_gameLoop(*tetrisGame);

                if (!base->running) {
                    tetris_freeGame(tetrisGame);
                    game.subGameManager.currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            default:
                log_error("Invalid GameScene_Et value: %d", game.subGameManager.currentScene);
                break;
        }
    }

    for (u8 i = 0; i < __gameSceneCount; ++i) {
        BaseGame_St** subGame = &game.subGameManager.miniGames[i];
        if ((*subGame) == NULL) continue;
        (*subGame)->freeGame(subGame);
    }

    lobby_freeApp();

    return 0;
}
