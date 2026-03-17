/**
    @file bowlingAPI.h
    @author Fshimi-Hawlk
    @date 2026-02-25
    @brief Public API for the Bowling mini-game.

    Defines the opaque game handle and the minimal lifecycle functions
    required to integrate Bowling into the lobby.

    @note The internal structure `BowlingGame_St` is **opaque** outside this module.
          Direct field access from the lobby or other modules is forbidden.

    @see generalAPI.h for the required `Game_St` base structure and `Error_Et` codes
*/

#ifndef BOWLING_API_H
#define BOWLING_API_H

#include "generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Opaque forward declaration — internal definition is private.
*/
typedef struct BowlingGame_St BowlingGame_St;

/**
    @brief Configuration parameters for Bowling initialization.

    Fields have safe defaults when zero-initialized.
*/
typedef struct {
    unsigned int fps;           ///< Target frame rate (0 = uncapped; common values: 60, 120, 144)
    // Future: difficulty preset, lane theme, ball customization, etc.
} BowlingConfigs_St;

/**
    @brief Convenience macro using C99 compound literal syntax.

    Example usage:
      BowlingGame_St* game = NULL;
      bowling_initGame(&game, .fps = 120);
*/
#define bowling_initGame(game, ...) \
    bowling_initGame__full((game), (BowlingConfigs_St){ .fps = 60, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Allocates and initializes a new Bowling game instance.

    @param[out] game     Double pointer receiving the new game handle (set to NULL on failure)
    @param[in]  configs  Initialization options

    @return OK on success
    @return ERROR_ALLOC on memory allocation failure
    @return other Error_Et codes on initialization failure

    @pre  *game == NULL
    @post On success: *game points to a valid game with base.running = true
    @post On failure: *game remains NULL

    @note Does **not** create/manage the Raylib window or context — lobby responsibility.
*/
Error_Et bowling_initGame__full(BowlingGame_St** game, BowlingConfigs_St configs);

/**
    @brief Runs one complete frame: input processing → logic update → rendering.

    Called once per frame when Bowling is the active mini-game.

    @param[in,out] game      Valid game instance handle

    @return OK on success
    @return ERROR_NULL_POINTER if game is NULL

    @pre  game != NULL and was successfully initialized
    @pre  Raylib drawing context is active (BeginDrawing() called)

    @note When game-over condition is reached, the function sets
          `((Game_St*)game)->running = false;`
*/
Error_Et bowling_gameLoop(BowlingGame_St* const game);

/**
    @brief Frees all resources owned by the game and releases the handle.

    @param[in,out] game  Pointer to the game handle (set to NULL after cleanup)

    @return OK on success
    @return ERROR_NULL_POINTER if *game is invalid (still sets to NULL)

    @pre  game may be NULL or point to a valid game
    @post *game == NULL
    @post All game-owned resources are freed

    @note Idempotent — safe to call multiple times.
    @note Does **not** close the Raylib window or call CloseWindow().
*/
Error_Et bowling_freeGame(BowlingGame_St** game);

#endif // BOWLING_API_H