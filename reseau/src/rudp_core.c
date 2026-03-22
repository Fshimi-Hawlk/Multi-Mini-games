/**
 * @file rudp_core.c
 * @brief Implémentation des mécanismes de base du protocole RUDP.
 * 
 * Ce fichier contient les fonctions permettant de gérer le séquencement 
 * des paquets, les acquittements via bitfield et la détection des doublons.
 * 
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 */

#include "rudp_core.h"
#include <string.h>

/**
 * @brief Initialise l'état d'une connexion RUDP.
 * 
 * @param conn Pointeur vers la structure de connexion à initialiser.
 */
void RUDP_InitConnection(RUDP_Connection *conn) {
    conn->local_sequence = 0;
    // Initialisé au max pour que le premier paquet (0) soit considéré comme "plus récent"
    conn->remote_sequence = 65535; 
    conn->receive_history = 0;
}

/**
 * @brief Vérifie si un numéro de séquence est chronologiquement après un autre.
 * 
 * Gère le bouclage des numéros de séquence (wrapping) sur 16 bits.
 * 
 * @param s1 Numéro de séquence à tester.
 * @param s2 Numéro de séquence de référence (dernier reçu).
 * @return true Si s1 est plus récent que s2.
 * @return false Sinon.
 */
static inline bool SequenceMoreRecent(u16 s1, u16 s2) {
    return ((s1 > s2) && (s1 - s2 <= 32768)) || ((s1 < s2) && (s2 - s1 > 32768));
}

/**
 * @brief Remplit les champs d'un en-tête RUDP avant l'envoi.
 * 
 * @param conn Pointeur vers l'état de la connexion.
 * @param action Type d'action à envoyer.
 * @param out_header Pointeur vers l'en-tête à remplir.
 */
void RUDP_GenerateHeader(RUDP_Connection *conn, u8 action, RUDP_Header *out_header) {
    out_header->sequence = conn->local_sequence++;
    out_header->ack = conn->remote_sequence;
    out_header->ack_bitfield = conn->receive_history;
    out_header->action = action;
    out_header->sender_id = 0; // Sera rempli par l'appelant (main.c ou server.c)
}

/**
 * @brief Analyse un en-tête entrant et met à jour l'historique de réception.
 * 
 * Implémente une fenêtre glissante d'acquittements via un bitfield de 32 bits.
 * 
 * @param conn Pointeur vers l'état de la connexion.
 * @param in_header En-tête du paquet reçu.
 * @return true Si le paquet est nouveau ou un retardataire non encore reçu.
 * @return false Si le paquet est un doublon ou trop ancien (hors fenêtre).
 */
bool RUDP_ProcessIncoming(RUDP_Connection *conn, const RUDP_Header *in_header) {
    u16 seq = in_header->sequence;

    // Cas 1 : Le paquet est plus récent que tout ce qu'on a vu
    if (SequenceMoreRecent(seq, conn->remote_sequence)) {
        u16 difference = (u16)(seq - conn->remote_sequence);
        
        if (difference >= HISTORY_SIZE) {
            conn->receive_history = 0;
        } else {
            conn->receive_history = (conn->receive_history << difference) | (1 << (difference - 1));
        }
        conn->remote_sequence = seq;
        return true;
    } 
    
    // Cas 2 : Le paquet est arrivé en retard (Jitter) mais n'a pas encore été reçu
    u16 diff_old = (u16)(conn->remote_sequence - seq);
    if (diff_old > 0 && diff_old <= HISTORY_SIZE) {
        u32 mask = 1U << (diff_old - 1);
        if (!(conn->receive_history & mask)) {
            conn->receive_history |= mask;
            return true; // Accepter le retardataire
        }
    }

    return false; // Trop vieux ou déjà traité
}