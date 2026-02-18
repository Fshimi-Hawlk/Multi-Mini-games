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
 * Games are loaded on demand via their API (e.g. tetrisAPI.h) and run
 * in the same process/window. No separate executables are spawned.
 */

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"


// ─────────────────────────────────────────────────────────────────────────────
// Program entry point
// ─────────────────────────────────────────────────────────────────────────────

int main(void) {
    
    // ── Main loop ────────────────────────────────────────────────────────────
    Error_Et error = OK;

    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    while (!WindowShouldClose()) {
        // float dt = GetFrameTime();

        switch (game->subGameManager.currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(game);
            } break;

            case GAME_SCENE_TETRIS: {
                Game_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];
                TetrisGame_St** tetrisRef = (TetrisGame_St**) miniRef;
                if (game->subGameManager.needGameInit) {
                    error = tetris_initGame(tetrisRef);
                    game->subGameManager.needGameInit = false;

                    if (error != OK) {
                        log_fatal("Tetris initialization failed: error %d", error);
                        tetris_freeGame(tetrisRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        break;
                    }
                }

                tetris_gameLoop(*tetrisRef);

                if (!(*miniRef)->running) {
                    tetris_freeGame(tetrisRef);
                    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            default:
                log_error("Invalid GameScene_Et value: %d", game->subGameManager.currentScene);
                break;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────────────────

    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"