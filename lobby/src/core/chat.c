#include "utils/globals.h"

#include "core/chat.h"

#include "lobbyAPI.h"

#include "networkInterface.h"
#include "APIs/generalAPI.h"

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
    
    // Make chat visible for 7 seconds
    msgVisibleTimer = 7.0f;
}

void updateChat(void) {
    float dt = GetFrameTime();
    if (msgVisibleTimer > 0) msgVisibleTimer -= dt;

    // Force close with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        lobby_game.chat.isOpen = false;
        return;
    }

    if (!lobby_game.chat.isOpen && IsKeyPressed(KEY_T)) {
        lobby_game.chat.isOpen = true;
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
                GameTLVHeader_St tlv = { .game_id = 0, .action = ACTION_CODE_LOBBY_CHAT, .length = (u16)strlen(lobby_game.chat.inputBuffer) + 1 };
                RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_CHAT, &h);
                
                u8 buffer[2048];
                size_t offset = 0;
                memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
                memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
                memcpy(buffer + offset, lobby_game.chat.inputBuffer, tlv.length); offset += (size_t)tlv.length;
                
                send(networkSocket, buffer, offset, 0);

                addChatMessage("Moi >", lobby_game.chat.inputBuffer);

                lobby_game.chat.inputPos = 0;
                lobby_game.chat.inputBuffer[0] = '\0';
            }
            lobby_game.chat.isOpen = false;
        }
    }
}

void drawChat(void) {
    // If chat is closed and no recent messages, only show hint
    if (!lobby_game.chat.isOpen && msgVisibleTimer <= 0) {
        DrawTextEx(lobby_fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.5f));
        return;
    }

    u32 width = 400; u32 height = 300; u32 x = 10; u32 y = GetScreenHeight() - height - 40;
    
    // Only draw background if open
    if (lobby_game.chat.isOpen) {
        DrawRectangle(x, y, width, height, Fade(BLACK, 0.1f));
        DrawRectangleLines(x, y, width, height, Fade(WHITE, 0.5f));
    }

    u32 start = (lobby_game.chat.count < MAX_CHAT_HISTORY) ? 0 : lobby_game.chat.head;
    float currentY = (float)y + 5;
    
    // When closed, only show last 5 messages
    u32 maxDisplay = lobby_game.chat.isOpen ? lobby_game.chat.count : 5;
    u32 displayStart = (lobby_game.chat.count > maxDisplay) ? (lobby_game.chat.count - maxDisplay) : 0;

    for (u32 i = displayStart; i < lobby_game.chat.count; ++i) {
        u32 idx = (start + i) % MAX_CHAT_HISTORY;
        ChatMessage_St* msg = &lobby_game.chat.messages[idx];
        const char* fullText = TextFormat("%s: %s", msg->sender, msg->text);
        
        Color textColor = WHITE;
        if (!lobby_game.chat.isOpen) {
            // Fade out based on timer
            float alpha = msgVisibleTimer > 1.0f ? 1.0f : msgVisibleTimer;
            textColor = Fade(WHITE, alpha);
        }

        Vector2 textSize = MeasureTextEx(lobby_fonts[FONT16], fullText, 16.0f, 0.0f);
        if (textSize.x > width - 10) {
            DrawTextEx(lobby_fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, 16.0f, 0.0f, textColor);
            currentY += 40;
        } else {
            DrawTextEx(lobby_fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, 16.0f, 0.0f, textColor);
            currentY += 20;
        }
        
        if (currentY > y + height - 40) break;
    }

    if (lobby_game.chat.isOpen) {
        DrawRectangle(x, y + height - 30, width, 30, Fade(BLACK, 0.3f));
        DrawRectangleLines(x, y + height - 30, width, 30, Fade(WHITE, 0.5f));
        DrawTextEx(lobby_fonts[FONT16], lobby_game.chat.inputBuffer, (Vector2){(float)x + 5, (float)y + height - 25}, 16.0f, 0.0f, WHITE);
    } else {
        DrawTextEx(lobby_fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.4f));
    }
}
