/**
    @file golfAPI.h
    @author sgit_v7
    @date 2026-04-01
    @brief Public API for the Golf 3D mini-game.

    Defines the opaque game handle and the minimal lifecycle functions
    required to integrate Golf 3D into the lobby.

    @note The internal structure `GolfGame_St` is opaque outside this module.
          Direct field access from the lobby or other modules is forbidden.

    @see generalAPI.h for the required `BaseGame_St` base structure and `Error_Et` codes
*/

#ifndef GOLF_API_H
#define GOLF_API_H

#include "APIs/generalAPI.h"

/* ─── Types ─────────────────────────────────────────────────────────────── */

/**
    @brief Opaque forward declaration — internal definition is private.
*/
typedef struct GolfGame_St GolfGame_St;

/**
    @brief Configuration parameters for Golf initialization.
*/
typedef struct {
    unsigned int fps;   ///< Target frame rate (0 = use default 60)
} GolfConfigs_St;

/**
    @brief Convenience macro using C99 compound literal syntax.
*/
#define golf_initGame(game, ...) \
    golf_initGame__full((game), (GolfConfigs_St){ .fps = 60, __VA_ARGS__ })

/* ─── Core lifecycle API ────────────────────────────────────────────────── */

/**
    @brief Allocates and initializes a new Golf 3D game instance.

    @param[out] game     Double pointer receiving the new game handle
    @param[in]  configs  Initialization options

    @return OK on success
    @return ERROR_ALLOC on memory allocation failure

    @pre  *game == NULL
    @post On success: *game points to a valid game with base.running = true
*/
Error_Et golf_initGame__full(GolfGame_St** game, GolfConfigs_St configs);

/**
    @brief Runs one complete frame: input → logic → rendering.

    @param[in,out] game  Valid game instance handle

    @return OK on success
    @return ERROR_NULL_POINTER if game is NULL

    @note Sets base.running = false when the player exits the scorecard (ESC/SPACE).
*/
Error_Et golf_gameLoop(GolfGame_St* const game);

/**
    @brief Frees all resources and releases the handle.

    @param[in,out] game  Pointer to game handle (set to NULL after cleanup)

    @return OK on success
    @return ERROR_NULL_POINTER if *game is invalid

    @post *game == NULL
    @note Idempotent — safe to call multiple times.
    @note Does NOT close the Raylib window.
*/
Error_Et golf_freeGame(GolfGame_St** game);

#endif /* GOLF_API_H */
