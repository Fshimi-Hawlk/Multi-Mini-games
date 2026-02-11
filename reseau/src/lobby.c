/**
 * @file lobby.c
 * @brief Implémentation de la logique de gestion du lobby.
 * * Gère les états des joueurs, les déplacements et les déconnexions.
 */

#include <stdlib.h> 
#include <string.h> 
#include "protocol.h"       
#include "game_interface.h" 

/**
 * @struct LobbyState
 * @brief État interne d'une instance de lobby.
 * * Contient les données de tous les joueurs connectés.
 */
typedef struct {
    Player_st players[MAX_CLIENTS]; /**< Tableau stockant les structures joueurs */
} LobbyState;

/**
 * @brief Alloue et initialise une nouvelle instance de lobby.
 * @return void* Pointeur vers la structure LobbyState créée.
 */
void* lobby_create() {
    LobbyState* state = (LobbyState*)malloc(sizeof(LobbyState));
    if (state) {
        memset(state, 0, sizeof(LobbyState));
    }
    return state;
}

/**
 * @brief Traite une action reçue d'un client.
 * * @param state Pointeur vers l'état du lobby.
 * @param player_id Index du joueur émetteur.
 * @param action Type d'action reçu.
 * @param payload Pointeur vers les données brutes.
 * @param len Taille des données reçues.
 * @param broadcast Fonction de rappel pour diffuser le message aux autres.
 */
void lobby_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    LobbyState *s = (LobbyState*)state;

    if (action == LOBBY_MOVE && len == sizeof(Player_st)) {
        Player_st *p = (Player_st*)payload;
        s->players[player_id] = *p;

        if (broadcast) {
            broadcast(0, player_id, LOBBY_MOVE, payload, len);
        }
    }
}

/**
 * @brief Gère le nettoyage des données lorsqu'un joueur quitte.
 * @param state Pointeur vers l'état du lobby.
 * @param player_id Index du joueur qui se déconnecte.
 */
void lobby_on_player_leave(void *state, int player_id) {
    LobbyState *s = (LobbyState*)state;
    memset(&s->players[player_id], 0, sizeof(Player_st));
}

/**
 * @brief Mise à jour logique du lobby (appelée à chaque frame serveur).
 * @param state Pointeur vers l'état du lobby.
 */
void lobby_tick(void *state) {
    (void)state; 
}

/**
 * @brief Libère la mémoire allouée pour l'instance du lobby.
 * @param state Pointeur vers l'état à détruire.
 */
void lobby_destroy(void *state) { 
    if (state) {
        free(state); 
    }
}

/**
 * @brief Exportation de l'interface du module pour le serveur.
 */
GameInterface lobby_module = {
    .game_name = "lobby",
    .create_instance = lobby_create,
    .on_action = lobby_on_action,
    .on_tick = lobby_tick,
    .destroy_instance = lobby_destroy,
    .on_player_leave = lobby_on_player_leave 
};