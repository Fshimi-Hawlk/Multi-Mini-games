/**
    @file gameNameAPI.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-02-23
    @brief Public API for <Game Name>.

    Defines the opaque game handle and the minimal lifecycle functions
    required to integrate GameName into the lobby.

    @note The internal structure `GameNameGame_St` is **opaque** outside this module.
          Direct field access from the lobby or other modules is forbidden.

    @see `generalAPI.h` for the required `Game_St` base structure and `Error_Et` codes
*/

#ifndef GAMENAME_API_H
#define GAMENAME_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Opaque forward declaration - internal definition is private.
*/
typedef struct GameNameGame_St GameNameGame_St;

/**
    @brief Configuration parameters for GameName initialization.

    Fields have safe defaults when zero-initialized.
*/
typedef struct {
    char __useless;             ///< Avoids warning for initGame macro. @note: Don't touch it!
    uint fps;           ///< Target frame rate (0 = uncapped; common values: 60, 120, 144)
    // other configs for the game
} GameNameConfigs_St;

/**
    @brief Convenience macro using C99 compound literal syntax.

    Example usage:
        GameNameGame_St* game = NULL;
        // with base configs
        gameName_initGame(&game);

        // with optional configs
        gameName_initGame(&game, .fps = 120);
*/
#define gameName_initGame(game, ...) \
    gameName_initGame__full((game), (GameNameConfigs_St){ .__useless = 0, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Allocates and initializes a new GameName game instance.

    @param[out] game     Double pointer receiving the new game handle (set to NULL on failure)
    @param[in]  configs      Initialization options

    @return OK on success
    @return ERROR_ALLOC on memory allocation failure
    @return other Error_Et codes on initialization failure

    @pre  *game == NULL
    @post On success: *game points to a valid game with base.running = true
    @post On failure: *game remains NULL

    @note Does **not** create/manage the Raylib window or context - lobby responsibility.
*/
Error_Et gameName_initGame__full(GameNameGame_St** game, GameNameConfigs_St configs);

/**
    @brief Runs one complete frame: input processing → logic update → rendering.

    Called once per frame when GameName is the active mini-game.

    @param[in,out] game      Valid game instance handle

    @return OK on success
    @return ERROR_NULL_POINTER if game is NULL

    @pre  game != NULL and was successfully initialized
    @pre  Raylib drawing context is active (BeginDrawing() called)

    @note When game-over condition is reached, the function sets
          `game.base->running = false;`
*/
Error_Et gameName_gameLoop(GameNameGame_St* const game);

/**
    @brief Frees all resources owned by the game and releases the handle.

    @param[in,out] game  Pointer to the game handle (set to NULL after cleanup)

    @return OK on success
    @return ERROR_NULL_POINTER if *game is invalid (still sets to NULL)

    @pre  game may be NULL or point to a valid game
    @post *game == NULL
    @post All game-owned resources are freed

    @note Idempotent - safe to call multiple times.
    @note Does **not** close the Raylib window or call CloseWindow().
*/
Error_Et gameName_freeGame(GameNameGame_St** game);

#endif // GAMENAME_API_H