/**
 * @file gameNameAPI.h
 * @author Fshimi-Hawlk
 * @date 2026-02-07                // Creation date
 * @date 2026-02-18                // Last meaningful changes (optional - remove if unchanged)
 * @brief Public API for the GameName mini-game.
 *
 * This header defines the opaque game handle type and the minimal set of functions
 * required to integrate and control the mini-game from the lobby.
 *
 * Design principles:
 *   - The internal structure `GameNameGame_St` is **completely opaque** outside this module.
 *   - The lobby interacts only through this API - never accesses fields directly.
 *   - All functions follow the `gameName_*` naming prefix.
 *   - Errors are reported using the shared `Error_Et` codes from generalAPI.h.
 *   - The game state embeds `Game_St` as its first member (for type-safe casting).
 *
 * @note Window creation, OpenGL context, and main loop timing are **not** managed by the game.
 *       The lobby is responsible for BeginDrawing()/EndDrawing(), BeginMode2D(), etc.
 *
 * @see generalAPI.h for the required `Game_St` base structure and error codes
 */

#ifndef GAME_NAME_API_H
#define GAME_NAME_API_H

/// @note: if you reading this file inside `firstparty/APIs` this following
///        include may have siggly-lines, but there's actually no real issue.
///        So, if any then just forget about it.
#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Forward declaration - internal game state definition is private.
 */
typedef struct GameNameGame_St GameNameGame_St;

/**
 * @brief Configuration parameters passed during initialization.
 *
 * All fields have safe defaults when zero-initialized.
 */
typedef struct {
    unsigned int fps;           ///< Target frame rate (0 = uncapped, common: 60 or 120)
    // Difficulty level, audio flags, seed for procedural generation, etc. can be added here
} GameNameConfigs_St;

/**
 * @brief Convenience macro for C99 compound literal initialization.
 *
 * Example:
 *   GameNameGame_St* game = NULL;
 *   gameName_initGame(&game, .fps = 144);
 */
#define gameName_initGame(game_ptr, ...) \
    gameName_initGame__full((game_ptr), (GameNameConfigs_St){ __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Allocates and initializes a new instance of the GameName mini-game.
 *
 * @param[out] game_ptr     Double pointer to receive the new game handle.
 *                          Set to NULL on failure.
 * @param[in]  configs      Initialization options (FPS, difficulty, etc.)
 *
 * @return OK on success
 * @return ERROR_ALLOC on memory allocation failure
 * @return other Error_Et codes for initialization failures (e.g. resource loading)
 *
 * @pre  *game_ptr == NULL
 * @post On success: *game_ptr points to a valid game object with base.running = true
 * @post On failure: *game_ptr remains NULL
 *
 * @note Does **not** create or manage the Raylib window/context.
 *       Caller (lobby) must handle InitWindow(), SetTargetFPS(), etc.
 */
Error_Et gameName_initGame__full(GameNameGame_St** game_ptr, GameNameConfigs_St configs);

/**
 * @brief Executes one full frame of the game: process input → update state → render.
 *
 * Should be called once per frame inside the lobby's main loop when this game is active.
 *
 * @param[in,out] game      Valid game instance handle
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if game is NULL
 * @return other Error_Et codes on runtime failures (rare)
 *
 * @pre  game != NULL and was successfully initialized
 * @pre  Raylib drawing context is active (BeginDrawing() called)
 *
 * @note If the game reaches an end condition (win/lose/quit), it must set
 *       `((Game_St*)game)->running = false;`
 */
Error_Et gameName_gameLoop(GameNameGame_St* const game);

/**
 * @brief Releases all resources owned by the game and frees the handle.
 *
 * @param[in,out] game_ptr  Pointer to the game handle. Set to NULL after cleanup.
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if *game_ptr is invalid (but still sets to NULL)
 *
 * @pre  game_ptr may be NULL or point to a valid game
 * @post *game_ptr == NULL
 * @post All game-owned resources (textures, sounds, internal arrays, etc.) are freed
 *
 * @note Idempotent - safe to call multiple times.
 * @note Does **not** close the Raylib window or call CloseWindow().
 */
Error_Et gameName_freeGame(GameNameGame_St** game_ptr);

#endif // GAME_NAME_API_H