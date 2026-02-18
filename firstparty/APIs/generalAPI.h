#ifndef GENERAL_API_H
#define GENERAL_API_H

/**
 * @file generalAPI.h
 * @author Fshimi Hawlk
 * @date 2026-01-08
 * @brief Shared core types and error codes used across all games and the lobby.
 *
 * This header defines types and enumerations that are considered "general"
 * and should be identical across all game implementations and the lobby.
 * It serves as the minimal common interface layer.
 */

#include <stdbool.h>

/**
 * @brief Standardized error codes returned by most initialization and operation functions.
 *
 * All game API functions that can fail should return one of these values.
 * The convention is: OK = 0, everything else is an error.
 */
typedef enum {
    OK = 0,                     ///< Operation completed successfully
    ERROR_NULL_POINTER,         ///< Required pointer argument was NULL
    ERROR_ALLOC,                ///< Memory allocation failed (malloc/calloc/realloc)
    ERROR_INVALID,              ///< Argument value is invalid/out of range/illegal state
    ERROR_TEXTURE_LOAD          ///< Texture couldn't load proprely
    // Add more specific codes later if needed, but keep them generic enough
} Error_Et;

/**
 * @brief Minimal common game state structure.
 *
 * Every game implementation must embed this structure at offset 0
 * (as the first member) so that a pointer cast to Game_St* remains valid.
 *
 * This allows the lobby to check basic state (especially .running)
 * without knowing the concrete game type.
 */
typedef struct {
    bool running;              ///< Whether the game is still active (true = continue, false = ended)
    // Future: add common fields here (e.g. score, timePlayed, paused, etc.)
    // Only add fields that truly belong to *every* game.
} Game_St;

#endif // GENERAL_API_H