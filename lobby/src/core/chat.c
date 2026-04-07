#include "utils/globals.h"
#include "networkInterface.h"
#include "core/chat.h"
#include "APIs/generalAPI.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static float msgVisibleTimer = 0.0f;

void addChatMessage(const char* sender, const char* text) {
    ChatMessage_St msg;
    strncpy(msg.sender, sender, MAX_NICKNAME_LEN - 1);
    msg.sender[MAX_NICKNAME_LEN - 1] = '\0';
    strncpy(msg.text, text, MAX_CHAT_MSG_LEN - 1);
    msg.text[MAX_CHAT_MSG_LEN - 1] = '\0';
    
    lobby_game.chat.messages[lobby_game.chat.head] = msg;
    lobby_game.chat.head = (lobby_game.chat.head + 1) % MAX_CHAT_HISTORY;
    if (lobby_game.chat.count < MAX_CHAT_HISTORY) lobby_game.chat.count++;
    
    msgVisibleTimer = 7.0f;
}

void updateChat(void) {
    float dt = GetFrameTime();
    if (msgVisibleTimer > 0) msgVisibleTimer -= dt;

    if (IsKeyPressed(KEY_ESCAPE) && lobby_game.chat.isOpen) {
        lobby_game.chat.isOpen = false;
        return;
    }

    if (!lobby_game.chat.isOpen && IsKeyPressed(KEY_T)) {
        lobby_game.chat.isOpen = true;
        GetCharPressed(); // Consume 'T'
    }

    if (lobby_game.chat.isOpen) {
        u32 key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (lobby_game.chat.inputPos < MAX_CHAT_MSG_LEN - 1)) {
                lobby_game.chat.inputBuffer[lobby_game.chat.inputPos++] = (char)key;
                lobby_game.chat.inputBuffer[lobby_game.chat.inputPos] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (lobby_game.chat.inputPos > 0) {
                lobby_game.chat.inputPos--;
                lobby_game.chat.inputBuffer[lobby_game.chat.inputPos] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (lobby_game.chat.inputPos > 0 && networkSocket != -1) {
                u16 payloadLen = (u16)strlen(lobby_game.chat.inputBuffer) + 1;
                GameTLVHeader_St tlv = { .game_id = 0, .action = ACTION_CODE_LOBBY_CHAT, .length = htons(payloadLen) };
                RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
                h.sender_id = htons((u16)lobby_game.id);
                
                u8 buffer[2048];
                size_t offset = 0;
                memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
                memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
                memcpy(buffer + offset, lobby_game.chat.inputBuffer, payloadLen); offset += (size_t)payloadLen;
                
                send(networkSocket, buffer, offset, 0);
                addChatMessage("Moi >", lobby_game.chat.inputBuffer);
                lobby_game.chat.inputPos = 0;
                lobby_game.chat.inputBuffer[0] = '\0';
            }
            lobby_game.chat.isOpen = false;
        }
    }
}

static void drawTextWrapped(Font font, const char* text, Vector2 pos, float width, float fontSize, float spacing, Color color) {
    int length = strlen(text);
    char buffer[MAX_CHAT_MSG_LEN + MAX_NICKNAME_LEN + 8];
    strncpy(buffer, text, sizeof(buffer)-1);
    
    int start = 0;
    float currentY = pos.y;

    while (start < length) {
        int end = start;
        int lastSpace = -1;
        while (end < length) {
            char saved = buffer[end + 1];
            buffer[end + 1] = '\0';
            Vector2 size = MeasureTextEx(font, buffer + start, fontSize, spacing);
            buffer[end + 1] = saved;

            if (size.x > width) break;
            if (buffer[end] == ' ') lastSpace = end;
            end++;
        }

        if (end < length && lastSpace != -1 && lastSpace > start) end = lastSpace;

        char saved = buffer[end];
        buffer[end] = '\0';
        DrawTextEx(font, buffer + start, (Vector2){pos.x, currentY}, fontSize, spacing, color);
        buffer[end] = saved;

        start = end;
        if (buffer[start] == ' ') start++;
        currentY += fontSize + 2;
    }
}

void drawChat(void) {
    if (!lobby_game.chat.isOpen && msgVisibleTimer <= 0) {
        DrawTextEx(lobby_fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.5f));
        return;
    }

    u32 width = 400; u32 height = 300; u32 x = 10; u32 y = GetScreenHeight() - height - 40;
    
    if (lobby_game.chat.isOpen || msgVisibleTimer > 0) {
        float alpha = lobby_game.chat.isOpen ? 0.4f : (msgVisibleTimer > 1.0f ? 0.4f : msgVisibleTimer * 0.4f);
        DrawRectangle(x, y, width, height, Fade(BLACK, alpha));
        if (lobby_game.chat.isOpen) DrawRectangleLines(x, y, width, height, Fade(WHITE, 0.5f));
    }

    u32 start = (lobby_game.chat.count < MAX_CHAT_HISTORY) ? 0 : lobby_game.chat.head;
    float currentY = (float)y + 5;
    u32 maxDisplay = lobby_game.chat.isOpen ? lobby_game.chat.count : 5;
    u32 displayStart = (lobby_game.chat.count > maxDisplay) ? (lobby_game.chat.count - maxDisplay) : 0;

    for (u32 i = displayStart; i < lobby_game.chat.count; ++i) {
        u32 idx = (start + i) % MAX_CHAT_HISTORY;
        ChatMessage_St* msg = &lobby_game.chat.messages[idx];
        const char* fullText = TextFormat("%s: %s", msg->sender, msg->text);
        
        Color textColor = WHITE;
        if (!lobby_game.chat.isOpen) {
            float alpha = msgVisibleTimer > 1.0f ? 1.0f : msgVisibleTimer;
            textColor = Fade(WHITE, alpha);
        }

        drawTextWrapped(lobby_fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, (float)width - 10, 16.0f, 0.0f, textColor);
        
        Vector2 size = MeasureTextEx(lobby_fonts[FONT16], fullText, 16.0f, 0.0f);
        int lines = (int)(size.x / (width - 10)) + 1;
        currentY += lines * 18;

        if (currentY > y + height - 40) break;
    }

    if (lobby_game.chat.isOpen) {
        DrawRectangle(x, y + height - 30, width, 30, Fade(BLACK, 0.3f));
        DrawRectangleLines(x, y + height - 30, width, 30, Fade(WHITE, 0.5f));
        DrawTextEx(lobby_fonts[FONT16], lobby_game.chat.inputBuffer, (Vector2){(float)x + 5, (float)y + height - 25}, 16.0f, 0.0f, WHITE);
    }
}
