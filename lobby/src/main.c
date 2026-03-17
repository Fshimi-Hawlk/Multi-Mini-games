/**
    @file main.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Program entry point – lobby main loop and game scene manager.
    
    This file contains the top-level application loop.
    It initializes the window and shared resources, runs the lobby,
    and switches to individual games when triggered (e.g. collision with zone).
    
    Games are loaded on demand via their API (e.g. bowlingAPI.h) and run
    in the same process/window. No separate executables are spawned.
    
    Supported mini-games:
        - Bowling: 3D bowling simulation triggered by entering the bowling zone
*/

#include "lobbyAPI.h"
#include "APIs/bowlingAPI.h"
#include "systemSettings.h"

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
 */
s32 main(void) {
    // ── Initialization ───────────────────────────────────────────────────────
    Error_Et error = OK;

    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {
        // float dt = GetFrameTime();

        switch (game->subGameManager.currentScene) {
            case GAME_SCENE_LOBBY: {
                lobby_gameLoop(game);
            } break;

            case GAME_SCENE_BOWLING: {
                BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BOWLING];
                BowlingGame_St** bowlingRef = (BowlingGame_St**) miniRef;
                if (game->subGameManager.needGameInit) {
                    error = bowling_initGame(bowlingRef);
                    game->subGameManager.needGameInit = false;

                    if (error != OK) {
                        log_fatal("Bowling initialization failed: error %d", error);
                        bowling_freeGame(bowlingRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        break;
                    }

                    systemSettings.video.width = BOWLING_SCREEN_WIDTH;
                    systemSettings.video.height = BOWLING_SCREEN_HEIGHT;

                    error = applySystemSettings();
                    if (error != OK) {
                        log_error("System settings couldn't be applied correctly");
                    }
                }

                bowling_gameLoop(*bowlingRef);

                if (!(*miniRef)->running) {
                    bowling_freeGame(bowlingRef);
                    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                    systemSettings = DEFAULT_SYSTEM_SETTING;
                    
                    error = applySystemSettings();
                    if (error != OK) {
                        log_error("System settings couldn't be applied correctly");
                    }
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

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
