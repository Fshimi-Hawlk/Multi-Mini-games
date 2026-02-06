// common/protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// --- HEADER TLV (Fixe) ---
// Chaque paquet commence par ça.
typedef struct __attribute__((packed)) {
    uint16_t room_id; // 0 = Lobby, 1-65535 = Jeux en cours
    uint8_t  action;  // ID de l'action (Dépend du jeu)
    uint16_t length;  // Taille du payload qui suit
} PacketHeader;

// --- IDS DES ACTIONS DU LOBBY ---
typedef enum {
    LOBBY_JOIN        = 1, // Un joueur arrive
    LOBBY_MOVE        = 2, // Un joueur bouge (Payload: LobbyMovePayload)
    LOBBY_CHAT        = 3, // Un joueur parle
    LOBBY_ENTER_GAME  = 4  // Le serveur dit: "Va dans la room X"
} LobbyAction;

// --- PAYLOADS (Données) ---

// Pour le mouvement dans le Lobby
typedef struct __attribute__((packed)) {
    float x;
    float y;
    int   anim_state; // 0=Idle, 1=Walk...
} LobbyMovePayload;

// Pour dire à un client de changer de salle
typedef struct __attribute__((packed)) {
    uint16_t new_room_id;
    char game_name[20];
} GameSwitchPayload;

#endif