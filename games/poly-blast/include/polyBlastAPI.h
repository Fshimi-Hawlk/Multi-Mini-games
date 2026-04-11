/**
    @file polyBlastAPI.h
    @author Fshimi-Hawlk
    @author Maxime-CHAUVEAU
    @date 2026-02-07
    @date 2026-02-23
    @brief Public API for the PolyBlast mini-game.

    This header defines the opaque game handle type and the minimal set of functions
    required to integrate and control the mini-game from the lobby.

    Design principles:
      - The internal structure `PolyBlastGame_St` is **completely opaque** outside this module.
      - The lobby interacts only through this API - never accesses fields directly.
      - All functions follow the `polyBlast_*` naming prefix.
      - Errors are reported using the shared `Error_Et` codes from generalAPI.h.
      - The game state embeds `Game_St` as its first member (for type-safe casting).

    @note Window creation, OpenGL context, and main loop timing are **not** managed by the game.
          The lobby is responsible for BeginDrawing()/EndDrawing(), BeginMode2D(), etc.

    @see generalAPI.h for the required `Game_St` base structure and error codes
    @see gameConfig.h for configuration options
*/

#ifndef POLY_BLAST_API_H
#define POLY_BLAST_API_H

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
typedef struct PolyBlastGame_St PolyBlastGame_St;

/**
    @brief Configuration parameters passed during initialization.

    All fields have safe defaults when zero-initialized.
*/
typedef struct {
    char _;         // make sure that using `polyBlast_initGame` doesn't cause any warning
    // Difficulty level, etc. can be added here
} PolyBlastConfigs_St;

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Convenience macro for C99 compound literal initialization.

    Example:
        PolyBlastGame_St* game = NULL;
        polyBlast_initGame(&game, .gameDifficulty = 144);
*/
#define polyBlast_initGame(game, ...) \
    polyBlast_initGame__full((game), (PolyBlastConfigs_St){ ._ = 0, __VA_ARGS__ })

/**
    @brief Allocates and initializes a new instance of the PolyBlast mini-game.

    @param[out] game     Double pointer to receive the new game handle.
                             Set to NULL on failure.
    @param[in]  configs      Video/Audio configuration (nothing => defaults)

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
Error_Et polyBlast_initGame__full(PolyBlastGame_St** game, const PolyBlastConfigs_St configs);

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
Error_Et polyBlast_gameLoop(PolyBlastGame_St* const game);

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
Error_Et polyBlast_freeGame(PolyBlastGame_St** game);

#endif // POLY_BLAST_API_H
