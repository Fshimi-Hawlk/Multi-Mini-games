/**
 * @file generalAPI.h
 * @author Fshimi-Hawlk
 * @date 2026-01-08
 * @date 2026-02-18
 * @brief Minimal shared interface and common types used by all mini-games and the lobby.
 *
 * This header defines:
 *   - a standard set of error codes used across game subsystems
 *   - the common base structure that every mini-game state must embed
 *   - a function pointer type for game cleanup
 *
 * Design goals:
 *   - Allow the lobby to interact with any mini-game in a type-safe, generic way
 *   - Enable uniform error handling across different game implementations
 *   - Keep the common interface as small as possible (only what truly belongs to every game)
 *
 * Important rules for mini-game implementations:
 *   - The concrete game state struct **must** have `Game_St base;` as its **first member**
 *   - This guarantees that `Game_St* base = (Game_St*)specificGame;` is always valid
 *   - The `freeGame` callback must free the entire game object (including the specific part)
 *
 * Typical usage in the lobby:
 *   - `Game_St* current = miniGames[scene];`
 *   - `if (current && current->running) { ... }`
 *   - On exit: `if (current && current->freeGame) current->freeGame(current);`
 *
 * @see lobby/include/utils/userTypes.h for SubGameManager_St (which holds pointers to Game_St*)
 */

#ifndef GENERAL_API_H
#define GENERAL_API_H

#include <stdbool.h>

// ────────────────────────────────────────────────
// Error codes (used project-wide)
// ────────────────────────────────────────────────

/**
 * @brief Standardized error codes for initialization, operations and resource loading.
 *
 * Convention:
 *   - OK = 0 means success
 *   - All other values indicate a specific failure reason
 *
 * Mini-games and subsystems should return one of these whenever possible.
 * More specific codes can be added later, but prefer generic ones when reasonable.
 */
typedef enum {
    OK = 0,                     ///< Operation succeeded
    ERROR_NULL_POINTER,         ///< Required pointer argument was NULL
    ERROR_ALLOC,                ///< Memory allocation failed
    ERROR_INVALID,              ///< Argument value or state is invalid/illegal
    ERROR_TEXTURE_LOAD,         ///< Failed to load a required texture
    // Future extension point: add more codes here as needed
} Error_Et;

// ────────────────────────────────────────────────
// Game interface
// ────────────────────────────────────────────────

/**
 * @brief Opaque forward declaration of the concrete game state.
 *
 * Actual definition lives in the specific mini-game header (.e.g. tetrisGame.h).
 */
typedef struct Game_St Game_St;

/**
 * @brief Function pointer type for game cleanup.
 *
 * Must free the entire game object (including any internal allocations)
 * and return an Error_Et code (usually OK).
 *
 * Signature: `Error_Et freeMyGame(void* game);`
 */
typedef Error_Et (*freeGame_Ft)(void*);

/**
 * @brief Common base structure that **every** mini-game state must embed as its first member.
 *
 * Layout guarantee:
 *   struct MyGame {
 *       Game_St base;
 *       // MyGame-specific fields...
 *   };
 *
 * This allows safe downcasting from `Game_St*` to concrete type only when needed,
 * while the lobby can safely access `running`, `paused`, `score`, and call `freeGame`.
 */
struct Game_St {
    bool        running;        ///< true = game should continue updating/rendering
    bool        paused;         ///< true = game is paused (no update, optional dimmed render)
    long        score;          ///< Score accumulated during the current/last session

    freeGame_Ft freeGame;       ///< Cleanup callback (must free the whole object)
};

#endif // GENERAL_API_H