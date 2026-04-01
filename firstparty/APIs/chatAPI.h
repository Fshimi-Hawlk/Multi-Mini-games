/**
 * @file chatAPI.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Common chat structures for the whole project.
 */

#ifndef CHAT_API_H
#define CHAT_API_H

#include "baseTypes.h"

#define MAX_CHAT_MSG_LEN 128 // longueur maximale d'un message de chat
#define MAX_CHAT_HISTORY 10 // nombre maximum de messages dans l'historique
#define MAX_NICKNAME_LEN 32 // longueur maximale du nom d'utilisateur


/**
 * @brief Structure représentant un message de chat.
 */

typedef struct {
    char sender[MAX_NICKNAME_LEN]; // nom d'utilisateur
    char text[MAX_CHAT_MSG_LEN]; // texte du message
} ChatMessage_St;

/**
 * @brief Structure représentant l'état du chat.
 */
typedef struct {
    ChatMessage_St  messages[MAX_CHAT_HISTORY]; // historique des messages
    u32             count; // nombre de messages dans l'historique
    u32             head; // index du prochain message à écrire dans l'historique
    char            inputBuffer[MAX_CHAT_MSG_LEN]; // buffer de saisie du message
    u32             inputPos; // position actuelle dans le buffer de saisie
    bool            isOpen; // indique si le chat est ouvert
} Chat_St;

#endif // CHAT_API_H
