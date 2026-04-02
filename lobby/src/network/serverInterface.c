/**
 * @file lobby.c
 * @brief Implémentation de la logique de gestion du lobby.
 * 
 * Gère les états des joueurs, les déplacements et les déconnexions 
 * au sein de l'espace de rencontre initial.
 * 
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 */

#include "utils/userTypes.h"
#include "utils/utils.h"

/**
 * @brief Alloue et initialise une nouvelle instance de lobby.
 * 
 * @return void* Pointeur vers la structure LobbyGame_St créée, ou NULL en cas d'échec.
 */
void* lobby_create(void) {
    return calloc(1, sizeof(LobbyGame_St));
}

/**
 * @brief Traite une action reçue d'un client.
 * 
 * Dans le lobby, les actions sont généralement diffusées à tous les autres clients.
 * 
 * @param state Pointeur vers l'état du lobby (LobbyGame_St).
 * @param player_id Index du joueur émetteur.
 * @param action Type d'action reçu.
 * @param payload Pointeur vers les données brutes.
 * @param len Taille des données reçues.
 * @param broadcast Fonction de rappel pour diffuser le message.
 */
void lobby_on_action(void *state, int player_id, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    LobbyGame_St *game = (LobbyGame_St*) state;

    if (action == ACTION_CODE_JOIN_GAME) {
        // Un client vient de rejoindre, on lui envoie la position de TOUS les autres
        for (s32 i = 0; i < MAX_CLIENTS; i++) {
            if (i != player_id && game->otherPlayers[i].active) {
                // On simule un message ACTION_CODE_LOBBY_MOVE venant du joueur i vers le nouveau client
                broadcast(player_id, i, ACTION_CODE_LOBBY_MOVE, &game->otherPlayers[i], sizeof(Player_St));
            }
        }
    } else if (action == ACTION_CODE_LOBBY_MOVE) {
        // On enregistre la position du joueur
        if (player_id >= 0 && player_id < MAX_CLIENTS && len == sizeof(Player_St)) {
            memcpy(&game->otherPlayers[player_id], payload, sizeof(Player_St));
            game->otherPlayers[player_id].active = true;
        }
        // Relay everything back to all clients in same room (target_id = UNICAST, original sender_id = player_id)
        broadcast(UNICAST, player_id, action, payload, len);
    }
    else {
        broadcast(UNICAST, player_id, action, payload, len);
    }
}

/**
 * @brief Gère le nettoyage des données lorsqu'un joueur quitte.
 * 
 * @param state Pointeur vers l'état du lobby.
 * @param player_id Index du joueur qui se déconnecte.
 */
void lobby_on_player_leave(void *state, int player_id) {
    LobbyGame_St* game = (LobbyGame_St*) state;
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        memset(&game->otherPlayers[player_id], 0, sizeof(Player_St));
    }
}

/**
 * @brief Mise à jour logique du lobby (appelée à chaque frame serveur).
 * 
 * @param state Pointeur vers l'état du lobby.
 */
void lobby_tick(void *state) {
    UNUSED(state); 
}

/**
 * @brief Libère la mémoire allouée pour l'instance du lobby.
 * 
 * @param state Pointeur vers l'état à détruire.
 */
void lobby_destroy(void *state) { 
    if (state) {
        free(state); 
    }
}

/**
 * @brief Interface du module lobby pour le serveur.
 * 
 * Expose les fonctions de gestion du cycle de vie et de traitement des messages.
 */
GameServerInterface_St lobbyServerInterface = {
    .game_name          = "lobby",
    .create_instance    = lobby_create,
    .on_action          = lobby_on_action,
    .on_tick            = lobby_tick,
    .destroy_instance   = lobby_destroy,
    .on_player_leave    = lobby_on_player_leave 
};