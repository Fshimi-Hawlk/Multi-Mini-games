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

    Architecture
    ────────────
    Instead of a growing switch/case, games are registered in the `scenes[]` table.
    Adding a new game requires only:
      1. A new GAME_SCENE_XXX entry in GameScene_Et (userTypes.h)
      2. A new row in scenes[] below
    The main loop is generic and never changes.
*/

#include "lobbyAPI.h"
#include "APIs/tetrisAPI.h"
#include "APIs/solitaireAPI.h"
#include "APIs/suikaAPI.h"
#include "APIs/bowlingAPI.h"
<<<<<<< HEAD
#include "systemSettings.h"
=======
#include "APIs/golfAPI.h"
#include "systemSettings.h"
#include "utils/configs.h"
>>>>>>> 3777fd6 (- add : new 3D golf game)

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
<<<<<<< HEAD
static Error_Et solitaire_loop_shim(BaseGame_St*  p){ return solitaire_gameLoop((SolitaireGame_St*)p), OK; }
=======
static Error_Et solitaire_loop_shim(BaseGame_St*  p){ solitaire_gameLoop((SolitaireGame_St*)p); return OK; }
>>>>>>> 3777fd6 (- add : new 3D golf game)
static Error_Et solitaire_free_shim(BaseGame_St** p){ return solitaire_freeGame((SolitaireGame_St**)p); }

static Error_Et suika_init_shim(BaseGame_St** p)    { return suika_initGame((SuikaGame_St**)p);         }
static Error_Et suika_loop_shim(BaseGame_St*  p)    { return suika_gameLoop((SuikaGame_St*)p);          }
static Error_Et suika_free_shim(BaseGame_St** p)    { return suika_freeGame((SuikaGame_St**)p);         }

static Error_Et bowling_init_shim(BaseGame_St** p)  { return bowling_initGame((BowlingGame_St**)p);     }
static Error_Et bowling_loop_shim(BaseGame_St*  p)  { return bowling_gameLoop((BowlingGame_St*)p);      }
static Error_Et bowling_free_shim(BaseGame_St** p)  { return bowling_freeGame((BowlingGame_St**)p);     }

<<<<<<< HEAD
=======
static Error_Et golf_init_shim(BaseGame_St** p)     { return golf_initGame((GolfGame_St**)p);           }
static Error_Et golf_loop_shim(BaseGame_St*  p)     { return golf_gameLoop((GolfGame_St*)p);            }
static Error_Et golf_free_shim(BaseGame_St** p)     { return golf_freeGame((GolfGame_St**)p);           }

>>>>>>> 3777fd6 (- add : new 3D golf game)
/**
    @brief Scene dispatch table.

    Index 0 = GAME_SCENE_LOBBY (handled separately — lobby is not a mini-game).
    Index i = GAME_SCENE_XXX.
*/
static const SceneDesc_St scenes[__gameSceneCount] = {
    [GAME_SCENE_LOBBY]    = { "Lobby",    NULL,               NULL,               NULL,               0,    0   },
    [GAME_SCENE_TETRIS]   = { "Tetris",   tetris_init_shim,   tetris_loop_shim,   tetris_free_shim,   600,  800 },
    [GAME_SCENE_SOLITAIRE]= { "Solitaire",solitaire_init_shim,solitaire_loop_shim,solitaire_free_shim,0,    0   },
    [GAME_SCENE_SUIKA]    = { "Suika",    suika_init_shim,    suika_loop_shim,    suika_free_shim,    800,  900 },
    [GAME_SCENE_BOWLING]  = { "Bowling",  bowling_init_shim,  bowling_loop_shim,  bowling_free_shim,  1200, 800 },
<<<<<<< HEAD
=======
    [GAME_SCENE_GOLF]     = { "Golf 3D",  golf_init_shim,     golf_loop_shim,     golf_free_shim,     1280, 720 },
>>>>>>> 3777fd6 (- add : new 3D golf game)
};

// ─────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────

static void returnToLobby(LobbyGame_St* game) {
    game->subGameManager.currentScene = GAME_SCENE_LOBBY;
<<<<<<< HEAD
    systemSettings = DEFAULT_SYSTEM_SETTING;
=======
    game->player.position = (Vector2){ 0, 440 };  // Teleport to center on lobby re-entry
    game->player.velocity = (Vector2){ 0, 0 };   // Reset velocity to prevent momentum carryover
    game->player.coyoteTimer = COYOTE_TIME;       // Reset jump states
    game->player.jumpBuffer = 0.0f;
    // Restore lobby window size (1200x800 is standard for lobby)
    systemSettings.video.width = 1200;
    systemSettings.video.height = 800;
    systemSettings.video.fps = 60;
    systemSettings.video.title = "Lobby";
>>>>>>> 3777fd6 (- add : new 3D golf game)
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
    LobbyGame_St* game = NULL;
    if (lobby_initGame(&game) == ERROR_ALLOC) {
        log_fatal("Couldn't load the lobby properly.");
        return 1;
    }

<<<<<<< HEAD
    while (!WindowShouldClose()) {
=======
    // Disable ESC to close window by default - games handle ESC to return to lobby
    SetExitKey(KEY_NULL);

    bool skipWindowClose = false;
    while (!WindowShouldClose() || skipWindowClose) {
        // Reset skip flag at start of each iteration
        if (skipWindowClose) skipWindowClose = false;

>>>>>>> 3777fd6 (- add : new 3D golf game)
        GameScene_Et scene = game->subGameManager.currentScene;

        // ── Lobby ────────────────────────────────────────────────
        if (scene == GAME_SCENE_LOBBY) {
<<<<<<< HEAD
=======
            // In lobby, ESC closes the game
            if (IsKeyPressed(KEY_ESCAPE) && WindowShouldClose()) {
                break;
            }
>>>>>>> 3777fd6 (- add : new 3D golf game)
            lobby_gameLoop(game);
            continue;
        }

<<<<<<< HEAD
=======
        // Re-disable ESC for games (in case we returned from lobby)
        SetExitKey(KEY_NULL);

>>>>>>> 3777fd6 (- add : new 3D golf game)
        // ── Mini-game generic dispatch ───────────────────────────
        if (scene >= __gameSceneCount || scenes[scene].init == NULL) {
            log_error("Invalid or unregistered GameScene_Et value: %d", scene);
            returnToLobby(game);
            continue;
        }

        const SceneDesc_St* desc    = &scenes[scene];
        BaseGame_St**       miniRef = &game->subGameManager.miniGames[scene];

        // Init (once)
        if (game->subGameManager.needGameInit) {
            Error_Et err = desc->init(miniRef);
            game->subGameManager.needGameInit = false;

            if (err != OK) {
                log_fatal("%s initialization failed (error %d)", desc->name, err);
                if (*miniRef) desc->free(miniRef);
                returnToLobby(game);
                continue;
            }
            applySceneWindowSize(desc);
        }

        // Loop frame
        if (*miniRef) desc->loop(*miniRef);

        // Detect end
        if (!*miniRef || !(*miniRef)->running) {
            if (*miniRef) desc->free(miniRef);
<<<<<<< HEAD
            returnToLobby(game);
=======
            game->subGameManager.needGameInit = true;  // Force reinit for next game
            returnToLobby(game);
            continue;
>>>>>>> 3777fd6 (- add : new 3D golf game)
        }
    }

    lobby_freeGame(&game);
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
