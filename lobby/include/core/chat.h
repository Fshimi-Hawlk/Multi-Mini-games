/**
    @file chat.h
    @author Léandre BAUDET
    @date 2026-03-18
    @date 2026-04-14
    @brief Multiplayer chat header.
*/
#ifndef LOBBY_CHAT_H
#define LOBBY_CHAT_H

/**
    @brief Description for addChatMessage
    @param[in,out] sender The sender parameter
    @param[in,out] text The text parameter
*/
void addChatMessage(const char* sender, const char* text);
/**
    @brief Description for updateChat
*/
void updateChat(void);

/**
    @brief Description for drawChat
*/
void drawChat(void);

#endif // LOBBY_CHAT_H