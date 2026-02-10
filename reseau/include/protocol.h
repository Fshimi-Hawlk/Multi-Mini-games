#ifndef PROTOCOL_H
#define PROTOCOL_H

// On inclut stdint.h pour avoir des types d'entiers à taille fixe (ex: uint16_t) peut important le pc
#include <stdint.h>

// --- CONSTANTES ---

#define MAX_CLIENTS 30 // Le nombre maximum de joueurs que le serveur accepte simultanément.

// --- STRUCTURE DU PAQUET --

// __attribute__((packed)) bloque le remplicage compilateur qui aligner la mémoire.

typedef struct __attribute__((packed)) {
    

    uint16_t room_id; // 2 octets (0 à 65535)

    uint16_t sender_id; // id du joueur qui fait l'action

    // Ex: 1 pour REJOINDRE, 2 pour BOUGER.
    uint8_t  action;  // 1 octet (0 à 255)

    // La taille des données qui suivent cet en-tête.
    uint16_t length;  // 2 octets (0 à 65535)

} PacketHeader; 

// TAILLE TOTALE = 2 + 1 + 2 = 5 Octets
/*
+-------------------------------------------------------+
|                 PacketHeader (5 octets)               |
+-------------------+---------+-------------------------+
|      room_id      |  action |         length          |
|    (uint16_t)     | (uint8_t)|      (uint16_t)        |
+---------+---------+---------+------------+------------+
| Octet 0 | Octet 1 | Octet 2 |   Octet 3  |   Octet 4  |
+---------+---------+---------+------------+------------+
*/

// --- TYPES D'ACTIONS ---

// Liste des commandes possibles.
typedef enum {

    LOBBY_JOIN = 1, 
    
    LOBBY_MOVE = 2  
} LobbyAction;

#endif


/*
[ EN-TÊTE (HEADER) ] -------------------------------------> [ CORPS (PAYLOAD) ]

      Room ID = 1        Action = 2       Length = 4          Position X, Y
   +----------------+----------------+----------------+   +-------------------+
   |  00000000  01  |    00000010    |  00000000  04  |   |  DATA (4 octets)  |
   +----------------+----------------+----------------+   +-------------------+
   
   ^ Le serveur lit d'abord ces 5 octets.
     Il voit "Length = 4".
     Il sait qu'il doit maintenant lire 4 octets de plus pour avoir la position.
*/