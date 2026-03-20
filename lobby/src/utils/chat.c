/**
 * @file chat.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Lobby chat integration implementation.
 */

#include "utils/chat.h"
#include "utils/globals.h"
#include "rudp_core.h"
#include "firstparty/APIs/generalAPI.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern int network_socket;
extern RUDP_Connection server_conn;

static float msgVisibleTimer = 0.0f;

void AddChatMessage(const char* sender, const char* text) {
    ChatMessage_St msg;
    strncpy(msg.sender, sender, MAX_NICKNAME_LEN - 1);
    msg.sender[MAX_NICKNAME_LEN - 1] = '\0';
    strncpy(msg.text, text, MAX_CHAT_MSG_LEN - 1);
    msg.text[MAX_CHAT_MSG_LEN - 1] = '\0';
    
    g_chatState.messages[g_chatState.head] = msg;
    g_chatState.head = (g_chatState.head + 1) % MAX_CHAT_HISTORY;
    if (g_chatState.count < MAX_CHAT_HISTORY) g_chatState.count++;
    
    // Make chat visible for 7 seconds
    msgVisibleTimer = 7.0f;
}

void UpdateChat(void) {
    float dt = GetFrameTime();
    if (msgVisibleTimer > 0) msgVisibleTimer -= dt;

    // Force close with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        g_chatState.isOpen = false;
        return;
    }

    if (!g_chatState.isOpen && IsKeyPressed(KEY_T)) {
        g_chatState.isOpen = true;
    }

    if (g_chatState.isOpen) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (g_chatState.inputPos < MAX_CHAT_MSG_LEN - 1)) {
                g_chatState.inputBuffer[g_chatState.inputPos++] = (char)key;
                g_chatState.inputBuffer[g_chatState.inputPos] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (g_chatState.inputPos > 0) {
                g_chatState.inputPos--;
                g_chatState.inputBuffer[g_chatState.inputPos] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (g_chatState.inputPos > 0 && network_socket != -1) {
                GameTLVHeader tlv = { .game_id = 0, .action = LOBBY_CHAT, .length = (uint16_t)strlen(g_chatState.inputBuffer) + 1 };
                RUDP_Header h; RUDP_GenerateHeader(&server_conn, LOBBY_CHAT, &h);
                
                uint8_t buffer[2048];
                size_t offset = 0;
                memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
                memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
                memcpy(buffer + offset, g_chatState.inputBuffer, tlv.length); offset += (size_t)tlv.length;
                
                send(network_socket, buffer, offset, 0);

                AddChatMessage("Moi", g_chatState.inputBuffer);

                g_chatState.inputPos = 0;
                g_chatState.inputBuffer[0] = '\0';
            }
            g_chatState.isOpen = false;
        }
    }
}

void DrawChat(void) {
    // If chat is closed and no recent messages, only show hint
    if (!g_chatState.isOpen && msgVisibleTimer <= 0) {
        DrawTextEx(fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.5f));
        return;
    }

    int width = 400; int height = 300; int x = 10; int y = GetScreenHeight() - height - 40;
    
    // Only draw background if open
    if (g_chatState.isOpen) {
        DrawRectangle(x, y, width, height, Fade(BLACK, 0.1f));
        DrawRectangleLines(x, y, width, height, Fade(WHITE, 0.5f));
    }

    int start = (g_chatState.count < MAX_CHAT_HISTORY) ? 0 : g_chatState.head;
    float currentY = (float)y + 5;
    
    // When closed, only show last 5 messages
    int maxDisplay = g_chatState.isOpen ? g_chatState.count : 5;
    int displayStart = (g_chatState.count > maxDisplay) ? (g_chatState.count - maxDisplay) : 0;

    for (int i = displayStart; i < g_chatState.count; ++i) {
        int idx = (start + i) % MAX_CHAT_HISTORY;
        ChatMessage_St* msg = &g_chatState.messages[idx];
        const char* fullText = TextFormat("%s: %s", msg->sender, msg->text);
        
        Color textColor = WHITE;
        if (!g_chatState.isOpen) {
            // Fade out based on timer
            float alpha = msgVisibleTimer > 1.0f ? 1.0f : msgVisibleTimer;
            textColor = Fade(WHITE, alpha);
        }

        Vector2 textSize = MeasureTextEx(fonts[FONT16], fullText, 16.0f, 0.0f);
        if (textSize.x > width - 10) {
            DrawTextEx(fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, 16.0f, 0.0f, textColor);
            currentY += 40;
        } else {
            DrawTextEx(fonts[FONT16], fullText, (Vector2){(float)x + 5, currentY}, 16.0f, 0.0f, textColor);
            currentY += 20;
        }
        
        if (currentY > y + height - 40) break;
    }

    if (g_chatState.isOpen) {
        DrawRectangle(x, y + height - 30, width, 30, Fade(BLACK, 0.3f));
        DrawRectangleLines(x, y + height - 30, width, 30, Fade(WHITE, 0.5f));
        DrawTextEx(fonts[FONT16], g_chatState.inputBuffer, (Vector2){(float)x + 5, (float)y + height - 25}, 16.0f, 0.0f, WHITE);
    } else {
        DrawTextEx(fonts[FONT16], "Press T to chat", (Vector2){10, (float)GetScreenHeight() - 30}, 16.0f, 0.0f, Fade(GRAY, 0.4f));
    }
}
