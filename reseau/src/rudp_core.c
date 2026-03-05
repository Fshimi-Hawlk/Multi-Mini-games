#include "rudp_core.h"
#include <string.h>

/**
 * @brief Initialise l'état de la connexion.
 */
void RUDP_InitConnection(RUDP_Connection *conn) {
    conn->local_sequence = 0;
    // Initialisé au max pour que le premier paquet (0) soit considéré comme "plus récent"
    conn->remote_sequence = 65535; 
    conn->receive_history = 0;
}

/**
 * @brief Calcule si s1 est chronologiquement après s2 sur un cycle de 65536.
 */
static inline bool SequenceMoreRecent(uint16_t s1, uint16_t s2) {
    return ((s1 > s2) && (s1 - s2 <= 32768)) || ((s1 < s2) && (s2 - s1 > 32768));
}

/**
 * @brief Remplit l'en-tête pour l'envoi.
 */
void RUDP_GenerateHeader(RUDP_Connection *conn, uint8_t action, RUDP_Header *out_header) {
    out_header->sequence = conn->local_sequence++;
    out_header->ack = conn->remote_sequence;
    out_header->ack_bitfield = conn->receive_history;
    out_header->action = action;
    out_header->sender_id = 0; // Sera rempli par l'appelant (main.c ou server.c)
}

/**
 * @brief Filtre les paquets entrants (rejets des vieux ou doublons).
 */
bool RUDP_ProcessIncoming(RUDP_Connection *conn, const RUDP_Header *in_header) {
    uint16_t seq = in_header->sequence;

    // Cas 1 : Le paquet est plus récent que tout ce qu'on a vu
    if (SequenceMoreRecent(seq, conn->remote_sequence)) {
        uint16_t difference = (uint16_t)(seq - conn->remote_sequence);
        
        if (difference >= HISTORY_SIZE) {
            conn->receive_history = 0;
        } else {
            conn->receive_history = (conn->receive_history << difference) | (1 << (difference - 1));
        }
        conn->remote_sequence = seq;
        return true;
    } 
    
    // Cas 2 : Le paquet est arrivé en retard (Jitter) mais n'a pas encore été reçu
    uint16_t diff_old = (uint16_t)(conn->remote_sequence - seq);
    if (diff_old > 0 && diff_old <= HISTORY_SIZE) {
        uint32_t mask = 1 << (diff_old - 1);
        if (!(conn->receive_history & mask)) {
            conn->receive_history |= mask;
            return true; // Accepter le retardataire
        }
    }

    return false; // Trop vieux ou déjà traité
}