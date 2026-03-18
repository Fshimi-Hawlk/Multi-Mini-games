/**
 * @file game_interface.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Definition of the generic interface for game modules.
 */

#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdint.h>

/**
 * @brief Prototype for the broadcast function provided by the server.
 * 
 * @param room_id The ID of the room to broadcast to.
 * @param exclude_id The ID of the player to exclude from the broadcast (usually the sender).
 * @param action The action code being broadcasted.
 * @param payload Pointer to the data to be sent.
 * @param len Length of the payload in bytes.
 */
typedef void (*broadcast_func_t)(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len);

/**
 * @struct GameInterface
 * @brief Structure of function pointers allowing abstraction of a game module.
 */
typedef struct GameInterface {
    char *game_name; /**< Name of the module (e.g., "lobby") */
    
    /**
     * @brief Initializes a new instance of the game.
     * @return Pointer to the newly created game state.
     */
    void* (*create_instance)();
    
    /**
     * @brief Processes a received network action.
     * @param state Pointer to the game state.
     * @param player_id ID of the player who sent the action.
     * @param action Action code received.
     * @param payload Pointer to the received data.
     * @param len Length of the payload.
     * @param broadcast Function pointer to broadcast messages to other players.
     */
    void  (*on_action)(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast);
    
    /**
     * @brief Logical update (tick).
     * @param state Pointer to the game state.
     */
    void  (*on_tick)(void *state);
    
    /**
     * @brief Handles a player leaving the game.
     * @param state Pointer to the game state.
     * @param player_id ID of the player who left.
     */
    void  (*on_player_leave)(void *state, int player_id);
    
    /**
     * @brief Cleans up and frees the game instance.
     * @param state Pointer to the game state to destroy.
     */
    void  (*destroy_instance)(void *state);
} GameInterface;

#endif
