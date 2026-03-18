/**
 * @file kingforfourAPI.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Public interface for the King-for-Four mini-game.
 * Standard Lobby integration (API Conversion).
 */

#ifndef KINGFORFOUR_API_H
#define KINGFORFOUR_API_H

#include "APIs/generalAPI.h"

/**
 * @struct KingForFourGame_St
 * @brief Opaque structure for King-for-Four game state.
 * The internal definition is private.
 */
typedef struct KingForFourGame_St KingForFourGame_St;

/**
 * @struct KingForFourConfigs_St
 * @brief Configuration parameters for game initialization.
 */
typedef struct {
    void *__useless;            /**< Unused placeholder. */
    unsigned int fps;           /**< Target frames per second. */
} KingForFourConfigs_St;

/**
 * @brief Convenience macro for game initialization with default 60 FPS.
 * @param game Pointer to game state pointer.
 * @param ... Additional configuration fields.
 */
#define kingforfour_initGame(game, ...) \
    kingforfour_initGame__full((game), (KingForFourConfigs_St){ .fps = 60, __VA_ARGS__ })

/**
 * @brief Allocates and initializes a King-for-Four game instance.
 * @param game_ptr Pointer to the location where the game state pointer will be stored.
 * @param configs Configuration parameters.
 * @return Error_Et status code.
 */
Error_Et kingforfour_initGame__full(KingForFourGame_St** game_ptr, KingForFourConfigs_St configs);

/**
 * @brief Executes a complete logic frame (Input -> Update -> Render).
 * @param game Pointer to the game state.
 * @return Error_Et status code.
 */
Error_Et kingforfour_gameLoop(KingForFourGame_St* const game);

/**
 * @brief Frees all resources allocated by the game.
 * @param game Pointer to the game state pointer to free.
 * @return Error_Et status code.
 */
Error_Et kingforfour_freeGame(KingForFourGame_St** game);

#endif // KINGFORFOUR_API_H
