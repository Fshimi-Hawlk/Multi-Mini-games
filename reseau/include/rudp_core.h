#ifndef RUDP_CORE_H
#define RUDP_CORE_H

#include <stdint.h>
#include <stdbool.h>

// --- CONFIGURATION LOBBY ---
#define MAX_CLIENTS 8

// --- ACTIONS RÉSEAU ---
#define LOBBY_JOIN 1
#define LOBBY_MOVE 2
#define LOBBY_ROOM_QUERY 3 
#define LOBBY_ROOM_INFO  4 

// --- CONSTANTES RUDP ---
#define MAX_SEQUENCE 65535
#define HISTORY_SIZE 32

// Désactivation du Padding pour un en-tête réseau de 9 octets (2+2+4+2+1)
#pragma pack(push, 1)
typedef struct {
    uint16_t sequence;     // ID du paquet
    uint16_t ack;          // Dernier paquet reçu
    uint32_t ack_bitfield; // Historique des 32 précédents
    uint16_t sender_id;    // ID du joueur (ESSENTIEL POUR LE RENDU)
    uint8_t  action;       // LOBBY_JOIN ou LOBBY_MOVE
} RUDP_Header;
#pragma pack(pop)

typedef struct {
    uint16_t local_sequence;       
    uint16_t remote_sequence;      
    uint32_t receive_history;      
} RUDP_Connection;

// API Publique
void RUDP_InitConnection(RUDP_Connection *conn);
void RUDP_GenerateHeader(RUDP_Connection *conn, uint8_t action, RUDP_Header *out_header);
bool RUDP_ProcessIncoming(RUDP_Connection *conn, const RUDP_Header *in_header);

#endif