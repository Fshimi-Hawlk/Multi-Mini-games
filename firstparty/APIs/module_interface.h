/**
 * @file module_interface.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Standardized interface for Client-side mini-games (Raylib Frontend).
 */

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <stdint.h>
#include <stddef.h>

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

#endif // MODULE_INTERFACE_H
