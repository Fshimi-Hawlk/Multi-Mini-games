/**
 * @file kingForFourAPI.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Public interface for the King-for-Four mini-game.
 * Integration compliant with the Lobby standard (API Conversion).
 */

#ifndef KINGFORFOUR_API_H
#define KINGFORFOUR_API_H

#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
 * @struct KingForFourGame_St
 * @brief Opaque forward declaration. The internal definition is private.
 */
typedef struct KingForFourGame_St KingForFourGame_St;

/**
 * @struct KingForFourConfigs_St
 * @brief Configuration parameters for initialization.
 */
typedef struct {
    void *__useless;            /**< Unused placeholder */
    unsigned int fps;           /**< Target frequency (frames per second) */
} KingForFourConfigs_St;

/**
 * @brief Convenience macro for initialization.
 */
#define kingforfour_initGame(game, ...) \
    kingforfour_initGame__full((game), (KingForFourConfigs_St){ .fps = 60, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// API de Cycle de Vie
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Allocates and initializes an instance of King-for-Four.
 * @param game_ptr Pointer to the pointer where the game instance will be stored.
 * @param configs Initialization configurations.
 * @return OK on success, or an error code.
 */
Error_Et kingforfour_initGame__full(KingForFourGame_St** game_ptr, KingForFourConfigs_St configs);

/**
 * @brief Executes a complete logic frame (Input -> Update -> Render).
 * @param game Pointer to the game instance.
 * @return OK on success, or an error code.
 */
Error_Et kingforfour_gameLoop(KingForFourGame_St* const game);

/**
 * @brief Frees all resources allocated by the game.
 * @param game Pointer to the pointer of the game instance.
 * @return OK on success, or an error code.
 */
Error_Et kingforfour_freeGame(KingForFourGame_St** game);

#endif // KINGFORFOUR_API_H
