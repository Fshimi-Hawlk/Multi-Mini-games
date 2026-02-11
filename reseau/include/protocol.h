/**
 * @file protocol.h
 * @brief Définition du protocole de communication réseau.
 * * Ce fichier contient les structures de paquets et les énumérations d'actions
 * partagées entre le client et le serveur.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

/** @brief Nombre maximum de joueurs acceptés simultanément sur le serveur. */
#define MAX_CLIENTS 30 

/**
 * @struct PacketHeader
 * @brief En-tête de tous les paquets réseau.
 * * L'attribut packed force le compilateur à ne pas ajouter de padding,
 * garantissant une taille de 5 octets pour l'en-tête.
 */
typedef struct __attribute__((packed)) {
    uint16_t room_id;   /**< ID de la salle (0-65535) */
    uint16_t sender_id; /**< ID du joueur émetteur de l'action */
    uint8_t  action;    /**< Type d'action (voir LobbyAction) */
    uint16_t length;    /**< Taille du corps (payload) qui suit l'en-tête */
} PacketHeader; 

/**
 * @enum LobbyAction
 * @brief Liste des types d'actions possibles dans le lobby.
 */
typedef enum {
    LOBBY_JOIN = 1, /**< Action pour rejoindre une salle */
    LOBBY_MOVE = 2  /**< Action pour mettre à jour la position */
} LobbyAction;

#endif