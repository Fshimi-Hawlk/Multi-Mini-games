/**
 * @file lobbyAPI.h
 * @author Fshimi Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Public API for the Lobby mini-game.
 *
 * This header defines the opaque game handle and the minimal set of functions
 * required to control the game from the lobby.
 *
 * @note The internal structure LobbyGame_St is **opaque** outside this module.
 *       The lobby and other modules must never access fields directly.
 *       If access to specific state is needed, add dedicated getter/setter
 *       functions to this API.
 */

#ifndef LOBBY_API_H
#define LOBBY_API_H

#include "utils/userTypes.h"

#include "APIs/generalAPI.h"

/**
 * @brief Configuration options passed to the initialization function.
 *
 * Default values should be sensible for standalone and lobby usage.
 */
typedef struct {
    unsigned int fps;          ///< Target frame rate (0 = uncapped, usually 60 or 120)
    // Add more options here when needed (difficulty, audio volume, window flags, ...)
} LobbyConfigs_St;

/**
 * @brief Convenience macro for C99 compound literal initialization syntax.
 *
 * Allows calls like:
 *   lobby_initGame(&game, .fps = 144);
 */
#define lobby_initGame(game, ...) \
    lobby_initGame__full(game, (LobbyConfigs_St){ .fps = 60, __VA_ARGS__ })

/* ────────────────────────────────────────────────────────────────────────────
   Core game lifecycle functions
   All functions follow the naming pattern: lobby_*
   ──────────────────────────────────────────────────────────────────────────── */

/**
 * @brief Allocates and initializes a new Lobby game instance.
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
Error_Et lobby_initGame__full(LobbyGame_St** game, LobbyConfigs_St configs);

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
Error_Et lobby_gameLoop(LobbyGame_St* const game);

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
 */
Error_Et lobby_freeGame(LobbyGame_St** game);

#endif // LOBBY_API_H