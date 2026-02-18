#ifndef GENERAL_API_H
#define GENERAL_API_H

/**
 * @file generalAPI.h
 * @author Fshimi Hawlk
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-01-08
 * @date 2026-03-18
 * @brief Shared core types and error codes used across all games and the lobby.
 *        Standardized interface for Client-side mini-games (Raylib Frontend).
 *
 * This header defines types and enumerations that are considered "general"
 * and should be identical across all game implementations and the lobby.
 * It serves as the minimal common interface layer.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

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

typedef struct Game_St Game_St;
typedef Error_Et (*freeGame_Ft)(void*);

/**
 * @brief Minimal common game state structure.
 *
 * Every game implementation must embed this structure at offset 0
 * (as the first member) so that a pointer cast to Game_St* remains valid.
 *
 * This allows the lobby to check basic state (especially .running)
 * without knowing the concrete game type.
 */
struct Game_St {
    bool running;               ///< Whether the game is still active (true = continue, false = ended)
    bool paused;                ///< Whether the game is paused or not
    long score;                 ///< Score gain during the last played game

    freeGame_Ft freeGame;       ///< Function to free the game's data

    // Only add fields that truly belong to *every* game.
};

/**
 * @struct GameTLVHeader
 * @brief Internal network structure (Payload).
 * Memory padding is disabled to match network bytes exactly (1+1+2 = 4 bytes).
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t game_id;     /**< ID of the targeted mini-game (0 for Lobby). */
    uint8_t action;      /**< Game-specific action (e.g., 2 for LOBBY_MOVE). */
    uint16_t length;     /**< Size of the data following this header. */
} GameTLVHeader;
#pragma pack(pop)

/**
 * @struct MiniGameModule
 * @brief Interface contract that each client mini-game must respect.
 */
typedef struct MiniGameModule {
    uint8_t id;          /**< Network identifier of the game. */
    const char* name;    /**< Display name of the module. */
    
    /**
     * @brief Allocation and loading of textures/models (Raylib).
     */
    void (*init)(void);
    
    /**
     * @brief Reception of a routed network packet for this game.
     * @param player_id ID of the player who sent the data.
     * @param action Action code.
     * @param data Pointer to the received data.
     * @param len Length of the data.
     */
    void (*on_data)(int player_id, uint8_t action, void* data, uint16_t len);
    
    /**
     * @brief Update of local physical logic (Frame by Frame).
     * @param dt Delta time since last frame.
     */
    void (*update)(float dt);
    
    /**
     * @brief Graphical rendering to the screen (Raylib).
     */
    void (*draw)(void);
} MiniGameModule;

#endif // GENERAL_API_H