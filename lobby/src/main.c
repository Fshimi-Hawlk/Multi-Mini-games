/**
    @file main.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-02-18
    @brief Program entry point – lobby main loop and game scene manager.
    
    Contributors:
        - LeandreB8:
            - Implemented basic lobby's logic (initialization, game loop, ...)
        - Fshimi-Hawlk:
            - Moved & reworked lobby's initialization, game loop and freeing logic in dedicated `lobbyAPI` files
            - Implememted sub-game playablity inside lobby logic via API
            - Added documentation
    
    This file contains the top-level application loop.
    It initializes the window and shared resources, runs the lobby,
    and switches to individual games when triggered (e.g. collision with zone).
    
    Games are loaded on demand via their API (e.g. suikaAPI.h) and run
    in the same process/window. No separate executables are spawned.
*/

#include "lobbyAPI.h"
#include "APIs/suikaAPI.h"
#include "systemSettings.h"

/**
    @brief Program entry point.
    @return 0 on clean exit, non-zero on early failure
*/
s32 main(void)
{
    // ── Initialization ───────────────────────────────────────────────────────
    Error_Et error = OK;

    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC)
    {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

    // ── Main loop ────────────────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        // float dt = GetFrameTime();

        switch (game->subGameManager.currentScene)
        {
            case GAME_SCENE_LOBBY:
            {
                lobby_gameLoop(game);
            } break;

            case GAME_SCENE_SUIKA:
            {
                BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_SUIKA];
                SuikaGame_St** suikaRef = (SuikaGame_St**) miniRef;
                if (game->subGameManager.needGameInit)
                {
                    error = suika_initGame(suikaRef);
                    game->subGameManager.needGameInit = false;

                    if (error != OK)
                    {
                        log_fatal("Suika initialization failed: error %d", error);
                        suika_freeGame(suikaRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        break;
                    }

                    systemSettings.video.width = 800;
                    systemSettings.video.height = 900;

                    error = applySystemSettings();
                    if (error != OK)
                    {
                        log_error("System settings couldn't be applied correctly");
                    }
                }

                suika_gameLoop(*suikaRef);

                if (!(*miniRef)->running)
                {
                    suika_freeGame(suikaRef);
                    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                    systemSettings = DEFAULT_SYSTEM_SETTING;
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

