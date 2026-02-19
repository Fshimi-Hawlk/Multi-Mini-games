/**
 * @file tetrisAPI.h
 * @author Fshimi-Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Public API for the Tetris mini-game.
 *
 * Defines the opaque game handle and the minimal lifecycle functions
 * required to integrate Tetris into the lobby.
 *
 * @note The internal structure `TetrisGame_St` is **opaque** outside this module.
 *       Direct field access from the lobby or other modules is forbidden.
 *
 * @see generalAPI.h for the required `Game_St` base structure and `Error_Et` codes
 */

#ifndef TETRIS_API_H
#define TETRIS_API_H

#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Opaque forward declaration — internal definition is private.
 */
typedef struct TetrisGame_St TetrisGame_St;

/**
 * @brief Configuration parameters for Tetris initialization.
 *
 * Fields have safe defaults when zero-initialized.
 */
typedef struct {
    unsigned int fps;           ///< Target frame rate (0 = uncapped; common values: 60, 120, 144)
    // Future: difficulty preset, hold piece enabled, ghost piece visibility, etc.
} TetrisConfigs_St;

/**
 * @brief Convenience macro using C99 compound literal syntax.
 *
 * Example usage:
 *   TetrisGame_St* game = NULL;
 *   tetris_initGame(&game, .fps = 120);
 */
#define tetris_initGame(game_ptr, ...) \
    tetris_initGame__full((game_ptr), (TetrisConfigs_St){ .fps = 60, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Allocates and initializes a new Tetris game instance.
 *
 * @param[out] game_ptr     Double pointer receiving the new game handle (set to NULL on failure)
 * @param[in]  configs      Initialization options
 *
 * @return OK on success
 * @return ERROR_ALLOC on memory allocation failure
 * @return other Error_Et codes on initialization failure
 *
 * @pre  *game_ptr == NULL
 * @post On success: *game_ptr points to a valid game with base.running = true
 * @post On failure: *game_ptr remains NULL
 *
 * @note Does **not** create/manage the Raylib window or context — lobby responsibility.
 */
Error_Et tetris_initGame__full(TetrisGame_St** game_ptr, TetrisConfigs_St configs);

/**
 * @brief Runs one complete frame: input processing → logic update → rendering.
 *
 * Called once per frame when Tetris is the active mini-game.
 *
 * @param[in,out] game      Valid game instance handle
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if game is NULL
 *
 * @pre  game != NULL and was successfully initialized
 * @pre  Raylib drawing context is active (BeginDrawing() called)
 *
 * @note When game-over condition is reached, the function sets
 *       `((Game_St*)game)->running = false;`
 */
Error_Et tetris_gameLoop(TetrisGame_St* const game);

/**
 * @brief Frees all resources owned by the game and releases the handle.
 *
 * @param[in,out] game_ptr  Pointer to the game handle (set to NULL after cleanup)
 *
 * @return OK on success
 * @return ERROR_NULL_POINTER if *game_ptr is invalid (still sets to NULL)
 *
 * @pre  game_ptr may be NULL or point to a valid game
 * @post *game_ptr == NULL
 * @post All game-owned resources are freed
 *
 * @note Idempotent — safe to call multiple times.
 * @note Does **not** close the Raylib window or call CloseWindow().
 */
Error_Et tetris_freeGame(TetrisGame_St** game_ptr);

#endif // TETRIS_API_H