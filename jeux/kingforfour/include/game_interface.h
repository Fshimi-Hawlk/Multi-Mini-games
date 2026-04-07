/**
 * @file game_interface.h
 * @brief Stub interface for network module (not currently used)
 */

#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdint.h>
#include <stddef.h>

typedef void (*broadcast_func_t)(int mode, int target_id, int channel, void* data, int len);

typedef void* (*create_instance_func_t)(void);
typedef void (*destroy_instance_func_t)(void* state);
typedef void (*on_action_func_t)(void* state, int player_id, uint8_t action, void* payload, uint16_t len, broadcast_func_t broadcast);
typedef void (*on_tick_func_t)(void* state, uint32_t tick);
typedef void (*on_player_leave_func_t)(void* state, int player_id);

typedef struct GameInterface {
    const char* game_name;
    create_instance_func_t create_instance;
    on_action_func_t on_action;
    on_tick_func_t on_tick;
    on_player_leave_func_t on_player_leave;
    destroy_instance_func_t destroy_instance;
} GameInterface;

#endif