/**
 * @file chat.h
 * @author i-Charlys (CAILLON Charles)
 * @author Fshimi Hawlk
 * @date 2026-03-18
 * @date 2026-03-19
 * @brief Multiplayer chat header.
 */

#ifndef LOBBY_CHAT_H
#define LOBBY_CHAT_H

#include "rudp_core.h"
#include "APIs/chatAPI.h"

extern s32 networkSocket;
extern RUDPConnection_St serverConn;

extern Chat_St gameChat;

void addChatMessage(const char* sender, const char* text);
void updateChat(void);

void drawChat(void);

#endif // LOBBY_CHAT_H