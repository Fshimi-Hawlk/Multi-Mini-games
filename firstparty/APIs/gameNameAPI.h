/**
 * @file gameNameAPI.h
 * @author Fshimi Hawlk
 * @date 2026-02-07
 * @brief Public API for the GameName mini-game.
 *
 * This header defines the opaque game handle and the minimal set of functions
 * required to control the game from the lobby.
 *
 * @note The internal structure GameNameGame_St is **opaque** outside this module.
 *       The lobby and other modules must never access fields directly.
 *       If access to specific state is needed, add dedicated getter/setter
 *       functions to this API.
 */

#ifndef GAMENAME_API_H
#define GAMENAME_API_H

#include "APIs/generalAPI.h"

/* Forward declaration — internal definition is private to the module */
typedef struct GameNameGame_St GameNameGame_St;

/**
 * @brief Configuration options passed to the initialization function.
 *
 * Default values should be sensible for standalone and lobby usage.
 */
typedef struct {
    unsigned int fps;          ///< Target frame rate (0 = uncapped, usually 60 or 120)
    // Add more options here when needed (difficulty, audio volume, window flags, ...)
} GameNameConfigs_St;

/**
 * @brief Convenience macro for C99 compound literal initialization syntax.
 *
 * Allows calls like:
 *   gameName_initGame(&game, .fps = 144);
 */
#define gameName_initGame(game, ...) \
    gameName_initGame__full(game, (GameNameConfigs_St){ __VA_ARGS__ })

/* ────────────────────────────────────────────────────────────────────────────
   Core game lifecycle functions
   All functions follow the naming pattern: gameName_*
   ──────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Allocates and initializes a new GameName game instance.
 *
 * @param[out] game         Double pointer to receive the allocated game handle.
 *                          Set to NULL on failure.
 * @param[in]  configs      Configuration values (FPS, difficulty, etc.)
 *
 * @return OK on success
 * @return ERROR_ALLOC if memory allocation failed
 * @return other Error_Et codes on other initialization failures
 *
 * @pre  *game == NULL
 * @post On success: *game points to a valid, initialized game object
 * @post On failure: *game remains NULL
 *
 * @note Does **not** initialize or manage the Raylib window/context.
 *       Window creation/cleanup is the responsibility of the caller (lobby).
 */
Error_Et gameName_initGame__full(GameNameGame_St** game, GameNameConfigs_St configs);

/**
 * @brief Executes one frame of the game (input -> update -> render).
 *
 * Should be called once per frame inside the main application loop.
 *
 * @param[in,out] game      Valid game instance (must not be NULL)
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if game is invalid
 *
 * @pre  game != NULL and game was successfully initialized
 * @pre  Raylib window and drawing context are active
 *
 * @note If the game decides to end (win/lose/quit), it should set
 *       ((Game_St*)game)->running = false;
 */
Error_Et gameName_gameLoop(GameNameGame_St* const game);

/**
 * @brief Releases all resources associated with the game and frees the handle.
 *
 * @param[in,out] game      Pointer to the game handle. Will be set to NULL.
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if game is invalid
 *
 * @pre  game may be NULL or point to a valid initialized game
 * @post *game == NULL
 * @post All game-allocated resources are freed
 *
 * @note Safe to call multiple times (idempotent).
 * @note Does **not** close the Raylib window or terminate the program.
 */
Error_Et gameName_freeGame(GameNameGame_St** game);

#endif // GAMENAME_API_H