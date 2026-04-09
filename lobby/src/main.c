/**
    @file main.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-03-22
    @brief Program entry point – lobby main loop and game scene manager.

    Contributors:
        - LeandreB8:
            - Implemented basic lobby's logic (initialization, game loop, ...)
        - Fshimi-Hawlk:
            - Moved & reworked lobby's init/loop/free in dedicated lobbyAPI files
            - Implemented sub-game playability via API
            - Added documentation

    Architecture
    ────────────
    Instead of a growing switch/case, games are registered in the `scenes[]` table.
    Adding a new game requires only:
      1. A new MINI_GAME_ID_XXX entry in MiniGameId_Et (userTypes.h)
      2. A new row in scenes[] below
    The main loop is generic and never changes.
*/

#include "APIs/generalAPI.h"

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"
#include "APIs/solitaireAPI.h"
#include "APIs/suikaAPI.h"
#include "APIs/bowlingAPI.h"
#include "APIs/golfAPI.h"
#include "APIs/snakeAPI.h"

// ─────────────────────────────────────────────────────────────────
// Generic game dispatch table
// ─────────────────────────────────────────────────────────────────

/**
    @brief Function pointer types matching the generalAPI lifecycle contract.
*/
typedef Error_Et (*InitFn)(BaseGame_St**);
typedef Error_Et (*LoopFn)(BaseGame_St*);
typedef Error_Et (*FreeFn)(BaseGame_St**);

/**
    @brief One entry per mini-game scene.

    width / height = 0 means "keep current window size".
*/
typedef struct {
    const char* name;
    InitFn      init;
    LoopFn      loop;
    FreeFn      free;
    int         width;
    int         height;
} SceneDesc_St;

// ── Adapter shims ────────────────────────────────────────────────
// Each game has its own typed pointer; we cast through BaseGame_St** here.
// This is safe because BaseGame_St is the first member of every game struct.

static Error_Et tetris_init_shim(BaseGame_St** p)   { return tetris_initGame((TetrisGame_St**)p);       }
static Error_Et tetris_loop_shim(BaseGame_St*  p)   { return tetris_gameLoop((TetrisGame_St*)p);        }
static Error_Et tetris_free_shim(BaseGame_St** p)   { return tetris_freeGame((TetrisGame_St**)p);       }

static Error_Et solitaire_init_shim(BaseGame_St** p){ return solitaire_initGame((SolitaireGame_St**)p); }
static Error_Et solitaire_loop_shim(BaseGame_St*  p){ solitaire_gameLoop((SolitaireGame_St*)p); return OK; }
static Error_Et solitaire_free_shim(BaseGame_St** p){ return solitaire_freeGame((SolitaireGame_St**)p); }

static Error_Et suika_init_shim(BaseGame_St** p)    { return suika_initGame((SuikaGame_St**)p);         }
static Error_Et suika_loop_shim(BaseGame_St*  p)    { return suika_gameLoop((SuikaGame_St*)p);          }
static Error_Et suika_free_shim(BaseGame_St** p)    { return suika_freeGame((SuikaGame_St**)p);         }

static Error_Et bowling_init_shim(BaseGame_St** p)  { return bowling_initGame((BowlingGame_St**)p);     }
static Error_Et bowling_loop_shim(BaseGame_St*  p)  { return bowling_gameLoop((BowlingGame_St*)p);      }
static Error_Et bowling_free_shim(BaseGame_St** p)  { return bowling_freeGame((BowlingGame_St**)p);     }

static Error_Et golf_init_shim(BaseGame_St** p)     { return golf_initGame((GolfGame_St**)p);           }
static Error_Et golf_loop_shim(BaseGame_St*  p)     { return golf_gameLoop((GolfGame_St*)p);            }
static Error_Et golf_free_shim(BaseGame_St** p)     { return golf_freeGame((GolfGame_St**)p);           }

static Error_Et snake_init_shim(BaseGame_St** p)    { return snake_initGame((SnakeGame_St**)p);          }
static Error_Et snake_loop_shim(BaseGame_St*  p)    { return snake_gameLoop((SnakeGame_St*)p);            }
static Error_Et snake_free_shim(BaseGame_St** p)    { return snake_freeGame((SnakeGame_St**)p);          }

/**
    @brief Scene dispatch table.

    Index 0 = MINI_GAME_ID_LOBBY (handled separately — lobby is not a mini-game).
    Index i = MINI_GAME_ID_XXX.
*/
static const SceneDesc_St scenes[__miniGameIdCount] = {
    [MINI_GAME_ID_LOBBY]    = { "Lobby",    NULL,               NULL,               NULL,               0,    0   },
    [MINI_GAME_ID_TETRIS]   = { "Tetris",   tetris_init_shim,   tetris_loop_shim,   tetris_free_shim,   600,  800 },
    [MINI_GAME_ID_SOLITAIRE]= { "Solitaire",solitaire_init_shim,solitaire_loop_shim,solitaire_free_shim,0,    0   },
    [MINI_GAME_ID_SUIKA]    = { "Suika",    suika_init_shim,    suika_loop_shim,    suika_free_shim,    800,  900 },
    [MINI_GAME_ID_BOWLING]  = { "Bowling",  bowling_init_shim,  bowling_loop_shim,  bowling_free_shim,  1200, 800 },
    [MINI_GAME_ID_GOLF]     = { "Golf 3D",  golf_init_shim,     golf_loop_shim,     golf_free_shim,     1280, 720 },
    [MINI_GAME_ID_SNAKE]       = { "Snake",      snake_init_shim,      snake_loop_shim,      snake_free_shim,       800,  600 },
};

// ─────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────

static void returnToLobby(LobbyGame_St* game) {
    game->subGameManager.currentScene = MINI_GAME_ID_LOBBY;
    game->player.position = (Vector2){ 0, 440 };  // Teleport to center on lobby re-entry
    game->player.velocity = (Vector2){ 0, 0 };   // Reset velocity to prevent momentum carryover
    game->player.coyoteTimer = COYOTE_TIME;       // Reset jump states
    game->player.jumpBuffer = 0.0f;
    // Restore lobby window size (1200x800 is standard for lobby)
    systemSettings.video.width = 1200;
    systemSettings.video.height = 800;
    systemSettings.video.fps = 60;
    systemSettings.video.title = "Lobby";
    applySystemSettings();
}

static void applySceneWindowSize(const SceneDesc_St* desc) {
    if (desc->width > 0 && desc->height > 0) {
        systemSettings.video.width  = desc->width;
        systemSettings.video.height = desc->height;
        applySystemSettings();
    }
}

// ─────────────────────────────────────────────────────────────────
// Entry point
// ─────────────────────────────────────────────────────────────────

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

    // Disable ESC to close window by default - games handle ESC to return to lobby
    SetExitKey(KEY_NULL);

    bool skipWindowClose = false;
    while (!WindowShouldClose() || skipWindowClose) {
        // Reset skip flag at start of each iteration
        if (skipWindowClose) skipWindowClose = false;

        MiniGameId_Et scene = game->subGameManager.currentScene;

        // ── Lobby ────────────────────────────────────────────────
        if (scene == MINI_GAME_ID_LOBBY) {
            // In lobby, ESC closes the game
            if (IsKeyPressed(KEY_ESCAPE) && WindowShouldClose()) {
                break;
            }
            lobby_gameLoop(game);
            continue;
        }

        // Re-disable ESC for games (in case we returned from lobby)
        SetExitKey(KEY_NULL);

        // ── Mini-game generic dispatch ───────────────────────────
        if (scene >= __miniGameIdCount || scenes[scene].init == NULL) {
            log_error("Invalid or unregistered MiniGameId_Et value: %d", scene);
            returnToLobby(game);
            continue;
        }

        const SceneDesc_St* desc    = &scenes[scene];
        BaseGame_St**       miniRef = &game->subGameManager.miniGames[scene];

        // Init (once)
        if (game->subGameManager.needGameInit) {
            error = desc->init(miniRef);
            game->subGameManager.needGameInit = false;

            if (error != OK) {
                log_fatal("%s initialization failed (error %d)", desc->name, error);
                if (*miniRef) {
                    error = desc->free(miniRef);
                }

                returnToLobby(game);
                continue;
            }
            applySceneWindowSize(desc);
        }

        // Loop frame
        if (*miniRef) {
            error = desc->loop(*miniRef);
            if (error != OK) log_warn("[%s] error: %d", desc->name, error);
        }

        // Detect end
        if (!*miniRef || !(*miniRef)->running) {
            if (*miniRef) desc->free(miniRef);
            game->subGameManager.needGameInit = true;  // Force reinit for next game
            returnToLobby(game);
            continue;
        }
    }

    lobby_freeGame(&game);

    return 0;
}

#define LOGGER_IMPLEMENTATION
#define PARAMS_MENU_IMPLEMENTATION
#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "paramsMenu.h"
