/**
 * @file chatAPI.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Common chat structures for the whole project.
 */

#ifndef CHAT_API_H
#define CHAT_API_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CHAT_MSG_LEN 128
#define MAX_CHAT_HISTORY 10
#define MAX_NICKNAME_LEN 32

typedef struct {
    char sender[MAX_NICKNAME_LEN];
    char text[MAX_CHAT_MSG_LEN];
} ChatMessage_St;

typedef struct {
    ChatMessage_St messages[MAX_CHAT_HISTORY];
    int count;
    int head;
    
    bool isOpen;
    char inputBuffer[MAX_CHAT_MSG_LEN];
    int inputPos;
} ChatState_St;

#endif // CHAT_API_H
