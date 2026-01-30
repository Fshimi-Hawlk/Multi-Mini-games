/**
 * @file userTypes.h
 * @author 
 * @date 
 * @brief Core type definitions used throughout the game.
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "utils/common.h"

typedef enum KeyCodes_e {
    KEY_NONE      = 0,
    KEY_ESC       = 27,
    KEY_ENTER     = 10,
    KEY_RETURN    = 13,
    KEY_BACKSPACE = 127,
    KEY_UP        = 1000,
    KEY_DOWN      = 1001,
    KEY_LEFT      = 1002,
    KEY_RIGHT     = 1003,
} KeyCode_Et;

typedef enum MessageTypes_e {
    MSG_BROADCAST = 0,
    MSG_WHISPER = 1,
    MSG_COMMAND = 2,
    MSG_COMMAND_RESPONSE = 3,
    MSG_USER_COUNT = 4,
} MessageType_Et;

typedef struct Message_s {
    MessageType_Et type;
    char sender[MAX_USERNAME_LENGTH];
    char target[MAX_USERNAME_LENGTH];  // for whisper
    char text[1024];
} Message_St;

typedef struct ClientInfo_s {
    s32     socketFd;
    char    receiveBuffer[RECEIVE_BUFFER_SIZE];
    size_t  receiveBufferUsedBytes;
    u64     expectedPayloadLength;
    char    nickname[MAX_USERNAME_LENGTH];
    bool    valid;
} ClientInfo_St;

typedef struct ChatHistory_s {
    Message_St messages[MAX_HISTORY];
    int count;
    int head;
} ChatHistory_St;

#endif // USER_TYPES_H