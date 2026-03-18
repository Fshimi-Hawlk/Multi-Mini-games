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
    
    Games are loaded on demand via their API (e.g. tetrisAPI.h) and run
    in the same process/window. No separate executables are spawned.
*/

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"
#include "APIs/snakeAPI.h"

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

            case GAME_SCENE_BATTLESHIP: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BATTLESHIP];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_BINGO: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BINGO];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_BLOCK_BLAST: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BLOCK_BLAST];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_BOWLING: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BOWLING];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_BRICK_BREAKER: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_BRICK_BREAKER];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_CONNECT_4: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_CONNECT_4];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_KFF: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_KFF];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_MINE_SWEEPER: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_MINE_SWEEPER];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_MINIGOLF: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_MINIGOLF];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_MORPION: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_MORPION];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_OTHELLO: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_OTHELLO];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_RUBIKS_CUBE: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_RUBIKS_CUBE];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_SNAKE: {
                BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_SNAKE];
                SnakeGame_St** snakeRef = (SnakeGame_St**) miniRef;
                if (game->subGameManager.needGameInit) {
                    error = snake_initGame(snakeRef);
                    game->subGameManager.needGameInit = false;

                    if (error != OK) {
                        log_fatal("<Game Name> initialization failed: error %d", error);
                        snake_freeGame(snakeRef);
                        game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        break;
                    }
                }

                snake_gameLoop(*snakeRef);

                if (!(*miniRef)->running) {
                    snake_freeGame(snakeRef);
                    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                }
            } break;

            case GAME_SCENE_SOLITAIRE: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_SOLITAIRE];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_SUIKA: {
                // BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_SUIKA];
                // <GameName>Game_St** <gameName>Ref = (<GameName>Game_St**) miniRef;
                // if (game->subGameManager.needGameInit) {
                    // error = <gameName>_initGame(<gameName>Ref);
                    // game->subGameManager.needGameInit = false;
// 
                    // if (error != OK) {
                        // log_fatal("<Game Name> initialization failed: error %d", error);
                        // <gameName>_freeGame(<gameName>Ref);
                        // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                        // break;
                    // }
                // }
// 
                // <gameName>_gameLoop(*<gameName>Ref);
// 
                // if (!(*miniRef)->running) {
                    // <gameName>_freeGame(<gameName>Ref);
                    // game->subGameManager.currentScene = GAME_SCENE_LOBBY;
                // }
            } break;

            case GAME_SCENE_TETRIS: {
                BaseGame_St** miniRef = &game->subGameManager.miniGames[GAME_SCENE_TETRIS];
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

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
