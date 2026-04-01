#include "utils/globals.h"
#include "networkInterface.h"

#include "core/chat.h"
#include "APIs/generalAPI.h"

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

static float msgVisibleTimer = 0.0f;

void addChatMessage(const char* sender, const char* text) {
    ChatMessage_St msg;
    strncpy(msg.sender, sender, MAX_NICKNAME_LEN - 1);
    msg.sender[MAX_NICKNAME_LEN - 1] = '\0';
    strncpy(msg.text, text, MAX_CHAT_MSG_LEN - 1);
    msg.text[MAX_CHAT_MSG_LEN - 1] = '\0';
    
    gameChat.messages[gameChat.head] = msg;
    gameChat.head = (gameChat.head + 1) % MAX_CHAT_HISTORY;
    if (gameChat.count < MAX_CHAT_HISTORY) gameChat.count++;
    
    // Make chat visible for 7 seconds
    msgVisibleTimer = 7.0f;
}

void updateChat(void) {
    float dt = GetFrameTime();
    if (msgVisibleTimer > 0) msgVisibleTimer -= dt;

    // Force close with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        gameChat.isOpen = false;
        return;
    }

    if (!gameChat.isOpen && IsKeyPressed(KEY_T)) {
        gameChat.isOpen = true;
        // Consume the 'T' key so it doesn't get typed
        GetCharPressed(); 
    }

    if (gameChat.isOpen) {
        u32 key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (gameChat.inputPos < MAX_CHAT_MSG_LEN - 1)) {
                gameChat.inputBuffer[gameChat.inputPos++] = (char)key;
                gameChat.inputBuffer[gameChat.inputPos] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (gameChat.inputPos > 0) {
                gameChat.inputPos--;
                gameChat.inputBuffer[gameChat.inputPos] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (gameChat.inputPos > 0 && networkSocket != -1) {
                GameTLVHeader_St tlv = { .game_id = 0, .action = ACTION_CODE_LOBBY_CHAT, .length = (u16)strlen(gameChat.inputBuffer) + 1 };
                RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_CHAT, &h);
                
                u8 buffer[2048];
                size_t offset = 0;
                memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
                memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
                memcpy(buffer + offset, gameChat.inputBuffer, tlv.length); offset += (size_t)tlv.length;
                
                send(networkSocket, buffer, offset, 0);

                addChatMessage("Moi >", gameChat.inputBuffer);

                gameChat.inputPos = 0;
                gameChat.inputBuffer[0] = '\0';
            }
            gameChat.isOpen = false;
        }
    }
}

static void drawTextWrapped(Font font, const char* text, Vector2 pos, float width, float fontSize, float spacing, Color color) {
    int length = strlen(text);
    char buffer[MAX_CHAT_MSG_LEN + 32];
    strcpy(buffer, text);
    
    int start = 0;
    int end = 0;
    float currentY = pos.y;

    while (start < length) {
        end = start;
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

        if (end < length && lastSpace != -1 && lastSpace > start) {
            end = lastSpace;
        }

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
    // Safety check for fonts
    if (!IsFontValid(lobby_fonts[FONT16])) {
        // Fallback to default font if lobby_fonts failed to load
        if (!gameChat.isOpen && msgVisibleTimer <= 0) {
            DrawText("Press T to chat", 10, GetScreenHeight() - 30, 16, Fade(GRAY, 0.5f));
            return;
        }
        // ... more complex fallback would be needed for full chat, but this prevents Segfault
        return;
    }

    // If chat is closed and no recent messages, only show hint

    u32 width = 400; u32 height = 300; u32 x = 10; u32 y = GetScreenHeight() - height - 40;
    float spacing = 0.0f;
    
    // Draw background 'voilage' if chat is open or there are recent messages
    if (gameChat.isOpen || msgVisibleTimer > 0) {
        float alpha = gameChat.isOpen ? 0.4f : (msgVisibleTimer > 1.0f ? 0.4f : msgVisibleTimer * 0.4f);
        DrawRectangle(x, y, width, height, Fade(BLACK, alpha));
        if (gameChat.isOpen) DrawRectangleLines(x, y, width, height, Fade(WHITE, 0.5f));
    }

    u32 start = (gameChat.count < MAX_CHAT_HISTORY) ? 0 : gameChat.head;
    float currentY = (float)y + 5;
    
    // When closed, only show last 5 messages
    u32 maxDisplay = gameChat.isOpen ? gameChat.count : 5;
    u32 displayStart = (gameChat.count > maxDisplay) ? (gameChat.count - maxDisplay) : 0;

    for (u32 i = displayStart; i < gameChat.count; ++i) {
        u32 idx = (start + i) % MAX_CHAT_HISTORY;
        ChatMessage_St* msg = &gameChat.messages[idx];
        const char* fullText = TextFormat("%s: %s", msg->sender, msg->text);
        
        Color textColor = WHITE;
        if (!gameChat.isOpen) {
            // Fade out based on timer
            float alpha = msgVisibleTimer > 1.0f ? 1.0f : msgVisibleTimer;
            textColor = Fade(WHITE, alpha);
        }

        drawTextWrapped(lobby_fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, (float)width - 10, 16.0f, spacing, textColor);
        
        // Dynamic Y advance (very approximate wrap count)
        Vector2 size = MeasureTextEx(lobby_fonts[FONT16], fullText, 16.0f, spacing);
        int lines = (int)(size.x / (width - 10)) + 1;
        currentY += lines * 18;

        if (currentY > y + height - 40) break;
    }

    if (gameChat.isOpen) {
        DrawRectangle(x, y + height - 30, width, 30, Fade(BLACK, 0.3f));
        DrawRectangleLines(x, y + height - 30, width, 30, Fade(WHITE, 0.5f));
        DrawTextEx(lobby_fonts[FONT16], gameChat.inputBuffer, (Vector2){(float)x + 5, (float)y + height - 25}, 16.0f, 0.0f, WHITE);
    } else {
        DrawTextEx(lobby_fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.4f));
    }
}
