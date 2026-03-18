/**
 * @file rudp_core.h
 * @brief Noyau du protocole RUDP (Reliable UDP) simplifié.
 * 
 * Définit les structures de paquets, les états de connexion et 
 * les fonctions de gestion du séquencement et de l'acquittement.
 * 
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 */

#ifndef RUDP_CORE_H
#define RUDP_CORE_H

#include <stdint.h>
#include <stdbool.h>

/*
 +-----------------------------------------------------------------------+
 |                          STRUCTURE D'UN PAQUET                        |
 +-----------------------------------------------------------------------+
 | [RUDP Header] (9 bytes) : Séquence, ACKs, Sender ID, Action RUDP      |
 +-----------------------------------------------------------------------+
 | [Game TLV]    (4 bytes) : Game ID, Action Jeu, Taille Payload         |
 +-----------------------------------------------------------------------+
 | [Payload]     (Variable): Données brutes (ex: Index carte, Position)  |
 +-----------------------------------------------------------------------+
*/

/**
 * @name Configuration Lobby
 * @{
 */
#define MAX_CLIENTS 8 /**< Nombre maximum de clients connectés simultanément. */
/** @} */

/**
 * @name Actions Réseau
 * @{
 */
#define LOBBY_JOIN 1      /**< Demande de connexion au lobby. */
#define LOBBY_MOVE 2      /**< Mise à jour de la position d'un joueur. */
#define LOBBY_ROOM_QUERY 3 /**< Requête de découverte de salle. */
#define LOBBY_ROOM_INFO  4  /**< Réponse d'information sur la salle. */
#define LOBBY_CHAT       5  /**< Message de chat. */
#define LOBBY_LEAVE      6   /**< Notification de déconnexion. */
#define LOBBY_SWITCH_GAME 0x20 /**< Demande de changement de module de jeu. */
/** @} */

/**
 * @name Constantes RUDP
 * @{
 */
#define MAX_SEQUENCE 65535 /**< Valeur maximale du numéro de séquence (16 bits). */
#define HISTORY_SIZE 32    /**< Taille du bitfield d'historique des acquittements. */
/** @} */

/**
 * @struct RUDP_Header
 * @brief En-tête du protocole RUDP (9 octets).
 * 
 * Contient les informations nécessaires pour garantir l'ordre et 
 * la fiabilité (partielle) de la transmission.
 */
#pragma pack(push, 1)
typedef struct {
    uint16_t sequence;     /**< ID unique du paquet. */
    uint16_t ack;          /**< Dernier numéro de séquence reçu avec succès. */
    uint32_t ack_bitfield; /**< Historique des 32 précédents paquets (1=reçu). */
    uint16_t sender_id;    /**< ID de l'émetteur (assigné par le serveur). */
    uint8_t  action;       /**< Type d'action RUDP ou Jeu. */
} RUDP_Header;
#pragma pack(pop)

/**
 * @struct RUDP_Connection
 * @brief État local d'une connexion RUDP.
 * 
 * Gère les numéros de séquence pour un canal de communication spécifique.
 */
typedef struct {
    uint16_t local_sequence;       /**< Prochain numéro de séquence à envoyer. */
    uint16_t remote_sequence;      /**< Dernier numéro de séquence reçu de l'autre côté. */
    uint32_t receive_history;      /**< Bitfield des paquets reçus. */
} RUDP_Connection;

/**
 * @brief Initialise une structure de connexion RUDP.
 * 
 * @param conn Pointeur vers la connexion à initialiser.
 */
void RUDP_InitConnection(RUDP_Connection *conn);

/**
 * @brief Génère un en-tête RUDP pour un nouvel envoi.
 * 
 * @param conn Pointeur vers l'état de la connexion.
 * @param action Type d'action à envoyer.
 * @param out_header Pointeur vers la structure d'en-tête à remplir.
 */
void RUDP_GenerateHeader(RUDP_Connection *conn, uint8_t action, RUDP_Header *out_header);

/**
 * @brief Traite un paquet entrant et met à jour l'historique.
 * 
 * Vérifie si le paquet est nouveau, un doublon ou trop ancien.
 * 
 * @param conn Pointeur vers l'état de la connexion.
 * @param in_header En-tête du paquet reçu.
 * @return true Si le paquet est valide et doit être traité.
 * @return false Si le paquet est un doublon ou périmé.
 */
bool RUDP_ProcessIncoming(RUDP_Connection *conn, const RUDP_Header *in_header);

#endif