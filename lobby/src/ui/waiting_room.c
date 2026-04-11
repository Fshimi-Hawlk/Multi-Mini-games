/**
 * @file waiting_room.c
 * @brief Waiting Room Overlay Implementation.
 */

#include "ui/waiting_room.h"
#include "networkInterface.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

static bool isPanelVisible = false;
static bool isInRoom = false;
static bool isHost = false;
static int curGameId = 0;
static int curRoomId = 0;
static int playerCount = 1;
static int maxPlayers = 4;

void initWaitingRoom(void) {
    isPanelVisible = false;
    isInRoom = false;
}

void showWaitingRoom(int gameId, int roomId, bool host) {
    isInRoom = true;
    isPanelVisible = true;
    curGameId = gameId;
    curRoomId = roomId;
    isHost = host;
}

void updateWaitingRoomInfo(int players, int max, bool host) {
    playerCount = players;
    maxPlayers = max;
    isHost = host;
}

void setWaitingRoomPanelVisible(bool visible) {
    isPanelVisible = visible;
}

void updateWaitingRoom(void) {
    if (!isInRoom) return;

    if (isPanelVisible) {
        // Handle X button
        Rectangle btnX = { (float)GetScreenWidth()/2 + 170, (float)GetScreenHeight()/2 - 190, 30, 30 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnX)) {
            isPanelVisible = false;
        }

        // Handle Ready/Start Button
        Rectangle btnAction = { (float)GetScreenWidth()/2 + 50, (float)GetScreenHeight()/2 + 150, 140, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnAction)) {
            if (isHost) {
                // Send Start Game to server
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_START_GAME, &h);
                u8 buf[64];
                memcpy(buf, &h, sizeof(h));
                int requestedPlayers = maxPlayers; 
                memcpy(buf + sizeof(h), &requestedPlayers, sizeof(int));

                send(networkSocket, buf, sizeof(h) + sizeof(int), 0);
                
                isPanelVisible = false;
            } else {
                // Send Ready to server
                // For now, just close panel
                isPanelVisible = false;
            }
        }

        // Handle Quit Button
        Rectangle btnQuit = { (float)GetScreenWidth()/2 - 190, (float)GetScreenHeight()/2 + 150, 140, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnQuit)) {
            isInRoom = false;
            isPanelVisible = false;
            // Switching back to lobby is handled by menus/pause
        }

        // Handle host max players selection
        if (isHost) {
            if (IsKeyPressed(KEY_UP) && maxPlayers < 4) maxPlayers++;
            if (IsKeyPressed(KEY_DOWN) && maxPlayers > 2) maxPlayers--;
        }
    } else {
        // Handle clicking status text to reopen
        Rectangle statusRect = { (float)GetScreenWidth()/2 - 120, 10, 240, 35 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), statusRect)) {
            isPanelVisible = true;
        }
    }
}

void drawWaitingRoom(void) {
    if (!isInRoom) return;

    if (isPanelVisible) {
        // Draw overlay
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));
        
        Rectangle panel = { (float)GetScreenWidth()/2 - 200, (float)GetScreenHeight()/2 - 200, 400, 400 };
        DrawRectangleRec(panel, RAYWHITE);
        DrawRectangleLinesEx(panel, 2, DARKGRAY);
        
        DrawText("SALLE D'ATTENTE", (int)panel.x + 100, (int)panel.y + 20, 20, BLACK);
        DrawText(TextFormat("Jeu ID: %d | Salon: %d", curGameId, curRoomId), (int)panel.x + 20, (int)panel.y + 50, 14, GRAY);

        // X Button
        DrawRectangle((int)panel.x + 370, (int)panel.y + 10, 20, 20, RED);
        DrawText("X", (int)panel.x + 375, (int)panel.y + 12, 16, WHITE);

        // Player List (Placeholder)
        DrawText("Joueurs:", (int)panel.x + 20, (int)panel.y + 80, 18, BLACK);
        DrawRectangle((int)panel.x + 20, (int)panel.y + 105, 360, 220, LIGHTGRAY);
        DrawText(TextFormat("- Vous (%s)", isHost ? "Hôte" : "Invité"), (int)panel.x + 30, (int)panel.y + 120, 18, DARKGRAY);
        if (playerCount > 1) {
            for (int i = 1; i < playerCount; i++) {
                DrawText(TextFormat("- Joueur %d", i + 1), (int)panel.x + 30, (int)panel.y + 120 + (i * 25), 18, DARKGRAY);
            }
        }

        // Buttons
        DrawRectangle((int)panel.x + 10, (int)panel.y + 350, 140, 40, ORANGE);
        DrawText("QUITTER", (int)panel.x + 40, (int)panel.y + 360, 18, WHITE);

        DrawRectangle((int)panel.x + 250, (int)panel.y + 350, 140, 40, isHost ? GOLD : GREEN);
        DrawText(isHost ? "LANCER" : "PRÊT", (int)panel.x + (isHost ? 275 : 290), (int)panel.y + 360, 18, WHITE);
        
        if (isHost) {
            DrawText("↑↓ pour changer max joueurs", (int)panel.x + 100, (int)panel.y + 330, 14, DARKGRAY);
            DrawText(TextFormat("Max: %d", maxPlayers), (int)panel.x + 300, (int)panel.y + 80, 18, BLUE);
        }
    } else {
        // Status text top center
        DrawRectangle(GetScreenWidth()/2 - 120, 10, 240, 35, Fade(RED, 0.8f));
        DrawText(TextFormat("Attente joueurs... (%d/%d)", playerCount, maxPlayers), 
                 GetScreenWidth()/2 - 110, 20, 18, WHITE);
    }
}
