/**
    @file bingoAPI.h
    @author Fshimi-Hawlk
    @date 2026-01-25
    @date 2026-03-19
    @brief Public API for <Game Name>.

    Defines the opaque game handle and the minimal lifecycle functions
    required to integrate Bingo into the lobby.

    @note The internal structure `BingoGame_St` is **opaque** outside this module.
          Direct field access from the lobby or other modules is forbidden.

    @see `generalAPI.h` for the required `Game_St` base structure and `Error_Et` codes
*/

#ifndef BINGO_API_H
#define BINGO_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Opaque forward declaration - internal definition is private.
*/
typedef struct BingoGame_St BingoGame_St;

/**
    @brief Configuration parameters for Bingo initialization.

    Fields have safe defaults when zero-initialized.
*/
typedef struct {
    u8 _;             ///< Avoids warning for initGame macro. @note: Don't touch it!
    // other configs for the game
} BingoConfigs_St;

/**
    @brief Convenience macro using C99 compound literal syntax.

    Example usage:
        BingoGame_St* game = NULL;
        // with base configs
        bingo_initGame(&game);

        // with optional configs
        bingo_initGame(&game, .fps = 120);
*/
#define bingo_initGame(game, ...) \
    bingo_initGame__full((game), (BingoConfigs_St){ ._ = 0, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// Core lifecycle API
// ────────────────────────────────────────────────────────────────────────────

/**
    @brief Allocates and initializes a new Bingo game instance.

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
Error_Et bingo_initGame__full(BingoGame_St** game, BingoConfigs_St configs);

/**
    @brief Runs one complete frame: input processing → logic update → rendering.

    Called once per frame when Bingo is the active mini-game.

    @param[in,out] game      Valid game instance handle

    @return OK on success
    @return ERROR_NULL_POINTER if game is NULL

    @pre  game != NULL and was successfully initialized
    @pre  Raylib drawing context is active (BeginDrawing() called)

    @note When game-over condition is reached, the function sets
          `game.base->running = false;`
*/
Error_Et bingo_gameLoop(BingoGame_St* const game);

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
Error_Et bingo_freeGame(BingoGame_St** game);

#endif // BINGO_API_H