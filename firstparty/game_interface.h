#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdint.h>

typedef void (*broadcast_func_t)(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len);

typedef struct GameInterface {
    char *game_name;
    void* (*create_instance)();
    // Signature avec 6 arguments (pour le broadcast)
    void  (*on_action)(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast);
    void  (*on_tick)(void *state);
    void  (*on_player_leave)(void *state, int player_id);
    void  (*destroy_instance)(void *state);
} GameInterface;

#endif


/*
+-----------------------------------------------------------+
|                  lobby_module (En Mémoire)                |
|                  Type: GameInterface                      |
+-----------------------------------------------------------+
|                                                           |
|  .game_name        ----->  "Reseau Module" (String)       |
|                                                           |
|  .create_instance  ----->  [ Pointeur vers fonction ]     |
|                            (Code dans lobby.c:init)       |
|                                                           |
|  .on_tick          ----->  [ Pointeur vers fonction ]     |
|                            (Code dans lobby.c:update)     |
|                                                           |
|  .destroy_instance ----->  [ Pointeur vers fonction ]     |
|                            (Code dans lobby.c:destroy)    |
|                                                           |
+-----------------------------------------------------------+
*/