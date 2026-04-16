/**
    @file snakeAPI.h
    @author Fshimi-Hawlk
    @date 2026-02-10
    @date 2026-04-14
    @brief Public API for the Snake mini-game.
*/
#ifndef SNAKE_API_H
#define SNAKE_API_H

/// @note: if you reading this file inside `firstparty/APIs` this following
///        include may have siggly-lines, but there's actually no real issue.
///        So, if any then just forget about it.
#include "baseTypes.h"
#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Forward declaration - internal game state definition is private.
*/
typedef struct SnakeGame_St SnakeGame_St;

/**
    @brief Configuration parameters passed during initialization.

    All fields have safe defaults when zero-initialized.
*/
typedef struct {
    char _;            ///< Avoids warning for initGame macro. @note: Don't touch it!
    // Difficulty level, etc. can be added here
} SnakeConfigs_St;

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Convenience macro for C99 compound literal initialization.

    Example:
        SnakeGame_St* game = NULL;
        snake_initGame(&game, .gameDifficulty = 144);
*/
#define snake_initGame(game, ...) \
    snake_initGame__full((game), (SnakeConfigs_St){ ._ = 0, __VA_ARGS__ })

/**
    @brief Allocates and initializes a new instance of the Snake mini-game.

    @param[out] game     Double pointer to receive the new game handle.
                             Set to NULL on failure.
    @param[in]  configs      Video/Audio configuration (NULL = defaults)

    @return OK on success
    @return ERROR_ALLOC on memory allocation failure
    @return ERROR_INVALID_CONFIG if configuration parameters are invalid
    @return other Error_Et codes for initialization failures (e.g. resource loading)

    @pre  *game == NULL
    @post On success: *game points to a valid game object with base.running = true
    @post On failure: *game remains NULL

    @note Does **not** create or manage the Raylib window/context.
          Caller (lobby) must handle InitWindow(), SetTargetFPS(), etc.

    @note If configs is NULL, the game uses default settings from gameConfig.h.
          Games should check configs->video and configs->audio pointers before accessing.
*/
Error_Et snake_initGame__full(SnakeGame_St** game, SnakeConfigs_St configs);

/**
    @brief Executes one full frame of the game: process input → update state → render.

    Should be called once per frame inside the lobby's main loop when this game is active.

    @param[in,out] game      Valid game instance handle

    @return OK on success
    @return ERROR_NULL_POINTER if game is NULL
    @return other Error_Et codes on runtime failures (rare)

    @pre  game != NULL and was successfully initialized
    @pre  Raylib drawing context is active (BeginDrawing() called)

    @note If the game reaches an end condition (win/lose/quit), it must set
          `game.base->running = false;`
*/
Error_Et snake_gameLoop(SnakeGame_St* const game);

/**
    @brief Releases all resources owned by the game and frees the handle.

    @param[in,out] game  Pointer to the game handle. Set to NULL after cleanup.

    @return OK on success
    @return ERROR_NULL_POINTER if *game is invalid (but still sets to NULL)

    @pre  game may be NULL or point to a valid game
    @post *game == NULL
    @post All game-owned resources (textures, sounds, internal arrays, etc.) are freed

    @note Idempotent - safe to call multiple times.
    @note Does **not** close the Raylib window or call CloseWindow().
*/
Error_Et snake_freeGame(SnakeGame_St** game);

/**
    @brief Checks if the game is still running.

    @param[in] game  Game instance handle (may be NULL)

    @return true if game is valid and running
    @return false if game is NULL or has stopped
*/
bool snake_isRunning(const SnakeGame_St* game);

/**
    @brief Updates the game state (input processing, logic).

    @param[in,out] game  Valid game instance handle
    @param[in] dt        Delta time in seconds
*/
void snake_update(SnakeGame_St* game, float dt);

/**
    @brief Renders the game (draw calls).

    @param[in] game  Valid game instance handle
*/
void snake_draw(const SnakeGame_St* game);

#endif // SNAKE_API_H
