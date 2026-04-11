/**
 * @file chat.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Lobby chat integration.
 */

#ifndef LOBBY_CHAT_H
#define LOBBY_CHAT_H

void UpdateChat(void);
void DrawChat(void);
void AddChatMessage(const char* sender, const char* text);

#endif // LOBBY_CHAT_H
