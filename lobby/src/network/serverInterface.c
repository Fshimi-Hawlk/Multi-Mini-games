/**
 * @file lobby.c
 * @brief Implémentation de la logique de gestion du lobby.
 * 
 * Gère les états des joueurs, les déplacements et les déconnexions 
 * au sein de l'espace de rencontre initial.
 * 
 * @author i-Charlys
 * @date 2026-03-18
 */

#include "utils/userTypes.h"

/**
 * @brief Alloue et initialise une nouvelle instance de lobby.
 * 
 * @return void* Pointeur vers la structure LobbyState créée, ou NULL en cas d'échec.
 */
void* lobby_create(void) {
    LobbyGame_St* state = malloc(sizeof(LobbyGame_St));
    if (state) {
        memset(state, 0, sizeof(LobbyGame_St));
    }

    return state;
}

/**
 * @brief Traite une action reçue d'un client.
 * 
 * Dans le lobby, les actions sont généralement diffusées à tous les autres clients.
 * 
 * @param state Pointeur vers l'état du lobby (LobbyState).
 * @param player_id Index du joueur émetteur.
 * @param action Type d'action reçu.
 * @param payload Pointeur vers les données brutes.
 * @param len Taille des données reçues.
 * @param broadcast Fonction de rappel pour diffuser le message.
 */
void lobby_on_action(void *state, int player_id, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    LobbyGame_St *s = (LobbyGame_St*)state;
    
    if (action == ACTION_CODE_LOBBY_MOVE && len >= sizeof(PlayerNet_St)) {
        PlayerNet_St net;
        memcpy(&net, payload, sizeof(PlayerNet_St));
        s->otherPlayers[player_id].position = (Vector2){ net.x, net.y };
        s->otherPlayers[player_id].angle = net.angle;
        s->otherPlayers[player_id].textureId = net.textureId;
        strncpy(s->otherPlayers[player_id].name, net.name, 31);
        s->otherPlayers[player_id].active = true;
    }

    if (action == ACTION_CODE_JOIN_GAME) {
        // Un nouveau joueur arrive, on lui envoie la position de tous les autres
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (s->otherPlayers[i].active && i != player_id) {
                PlayerNet_St other;
                other.x = s->otherPlayers[i].position.x;
                other.y = s->otherPlayers[i].position.y;
                other.angle = s->otherPlayers[i].angle;
                other.textureId = s->otherPlayers[i].textureId;
                other.active = true;
                strncpy(other.name, s->otherPlayers[i].name, 31);
                
                // Envoi ciblé au nouveau venu. 
                // On fait passer le message comme venant de 'i' (le joueur déjà là)
                broadcast(-(player_id + 1), i, ACTION_CODE_LOBBY_MOVE, &other, sizeof(PlayerNet_St));
            }
        }
    }

    // Diffusion standard
    broadcast(0, player_id, action, payload, len);
}

/**
 * @brief Gère le nettoyage des données lorsqu'un joueur quitte.
 * 
 * @param state Pointeur vers l'état du lobby.
 * @param player_id Index du joueur qui se déconnecte.
 */
void lobby_on_player_leave(void *state, int player_id) {
    LobbyGame_St *s = (LobbyGame_St*)state;
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        memset(&s->otherPlayers[player_id], 0, sizeof(Player_St));
    }
}

/**
 * @brief Mise à jour logique du lobby (appelée à chaque frame serveur).
 * 
 * @param state Pointeur vers l'état du lobby.
 */
void lobby_tick(void *state) {
    (void)state; 
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
GameServerInterface_St lobby_module = {
    .game_name = "lobby",
    .create_instance = lobby_create,
    .on_action = lobby_on_action,
    .on_tick = lobby_tick,
    .destroy_instance = lobby_destroy,
    .on_player_leave = lobby_on_player_leave 
};