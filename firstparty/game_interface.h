/**
 * @file game_interface.h
 * @brief Définition de l'interface générique pour les modules de jeu.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-02-07
 */

#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdint.h>

/**
 * @brief Prototype de la fonction de broadcast fournie par le serveur.
 */
typedef void (*broadcast_func_t)(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len);

/**
 * @struct GameInterface
 * @brief Structure de fonctions pointées permettant d'abstraire un module de jeu.
 */
typedef struct GameInterface {
    char *game_name; /**< Nom du module (ex: "lobby") */
    
    /** @brief Initialise une nouvelle instance du jeu. */
    void* (*create_instance)();
    
    /** @brief Traite une action réseau reçue. */
    void  (*on_action)(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast);
    
    /** @brief Mise à jour logique (tick). */
    void  (*on_tick)(void *state);
    
    /** @brief Gère le départ d'un joueur. */
    void  (*on_player_leave)(void *state, int player_id);
    
    /** @brief Nettoie et libère l'instance. */
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