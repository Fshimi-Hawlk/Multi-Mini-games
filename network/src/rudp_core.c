/**
 * @file rudp_core.c
 * @brief Cœur du protocole Reliable UDP (RUDP) simplifié.
 * 
 * Ce module gère la couche de transport fiable au-dessus de l'UDP standard.
 * Il assure le séquencement des paquets, la détection des doublons via une fenêtre
 * glissante et fournit les mécanismes d'acquittement cumulatif (ACK + Bitfield).
 * 
 * @note L'en-tête RUDP (RUDPHeader_St) occupe exactement 11 octets sur le réseau.
 * 
 * @author i-Charlys
 * @date 2026-03-18
 */

#include "rudp_core.h"
#include <arpa/inet.h>

/**
 * @brief Initialise l'état d'une session RUDP.
 * 
 * Le numéro de séquence local démarre à 0.
 * Le numéro distant est initialisé à 65535 pour que le premier paquet reçu (0)
 * soit correctement identifié comme plus récent via SequenceMoreRecent.
 * 
 * @param conn État de la connexion à réinitialiser.
 */
void rudpInitConnection(RUDPConnection_St *conn) {
    conn->local_sequence = 0;
    conn->remote_sequence = 65535; 
    conn->receive_history = 0;
}

/**
 * @brief Compare deux numéros de séquence en gérant le bouclage (wrapping) sur 16 bits.
 * 
 * Un numéro est considéré comme plus récent s'il se situe dans la moitié positive
 * de l'espace des séquences par rapport à la référence.
 * 
 * @param s1 Séquence à tester.
 * @param s2 Séquence de référence (dernier paquet reçu).
 * @return true si s1 est chronologiquement après s2.
 */
static inline bool sequenceMoreRecent(u16 s1, u16 s2) {
    return (u16)(s1 - s2) < 32768 && (s1 != s2);
}

/**
 * @brief Prépare l'en-tête d'un paquet sortant.
 * 
 * Incrémente la séquence locale et inclut les informations d'acquittement 
 * courant (remote_sequence et historique) pour informer le pair de l'état de notre réception.
 * 
 * @param conn   État actif de la connexion.
 * @param action Code d'action (Lobby ou Jeu).
 * @param out_h  Structure d'en-tête à remplir.
 */
void rudpGenerateHeader(RUDPConnection_St *conn, u8 action, RUDPHeader_St *out_h) {
    out_h->sequence     = htons(conn->local_sequence++);
    out_h->ack          = htons(conn->remote_sequence);
    out_h->ack_bitfield = htonl(conn->receive_history);
    out_h->action       = action;
    out_h->sender_id    = 0; // Défini ultérieurement par le serveur ou le main client
}

/**
 * @brief Analyse un paquet entrant et met à jour l'état de réception local.
 * 
 * Gère deux cas :
 * 1. Paquet plus récent : Décale l'historique et met à jour la séquence de référence.
 * 2. Paquet en retard (jitter) : Vérifie dans l'historique s'il est manquant et l'accepte si oui.
 * 
 * @param conn État de la connexion à mettre à jour.
 * @param in_h En-tête du paquet reçu (en format réseau).
 * @return true si le paquet est nouveau ou nécessaire (doit être traité), 
 *         false s'il s'agit d'un doublon ou s'il est hors fenêtre.
 */
bool rudpProcessIncoming(RUDPConnection_St *conn, const RUDPHeader_St *in_h) {
    u16 seq = ntohs(in_h->sequence);
    u16 ack = ntohs(in_h->ack);
    u32 bitfield = ntohl(in_h->ack_bitfield);

    /* 
       Traitement des ACKs du pair :
       Le pair nous dit qu'il a reçu jusqu'à 'ack' et les paquets du 'bitfield'.
       Ici, on pourrait nettoyer une file de retransmission locale.
    */
    (void)ack;
    (void)bitfield;

    // Cas 1 : Le paquet est plus récent que tout ce qu'on a vu
    if (sequenceMoreRecent(seq, conn->remote_sequence)) {
        u16 difference = (u16)(seq - conn->remote_sequence);
        
        if (difference >= HISTORY_SIZE) {
            conn->receive_history = 0;
        } else {
            // Décale l'historique et marque le précédent remote_sequence comme reçu (bit 0)
            conn->receive_history = (conn->receive_history << difference) | ((u32)1 << (difference - 1));
        }
        conn->remote_sequence = seq;
        return true;
    } 
    
    // Cas 2 : Le paquet est arrivé en retard mais dans la fenêtre de l'historique
    u16 diff_old = (u16)(conn->remote_sequence - seq);
    if (diff_old > 0 && diff_old <= HISTORY_SIZE) {
        u32 mask = 1U << (diff_old - 1);
        if (!(conn->receive_history & mask)) {
            conn->receive_history |= mask;
            return true; // Paquet retardataire accepté car non reçu précédemment
        }
    }

    return false; // Doublon ou trop vieux
}