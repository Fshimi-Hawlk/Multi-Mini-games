/**
    @file chat.h
    @author Léandre BAUDET
    @date 2026-03-18
    @date 2026-04-14
    @brief Lobby chat integration.
*/
#ifndef LOBBY_CHAT_H
#define LOBBY_CHAT_H

/**
    @brief Description for UpdateChat
*/
void UpdateChat(void);
/**
    @brief Description for DrawChat
*/
void DrawChat(void);
/**
    @brief Description for AddChatMessage
    @param[in,out] sender The sender parameter
    @param[in,out] text The text parameter
*/
void AddChatMessage(const char* sender, const char* text);

#endif // LOBBY_CHAT_H
