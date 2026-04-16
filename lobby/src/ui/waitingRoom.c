/**
    @file ui/waitingRoom.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Overlay UI for waiting rooms (modal panel + status text).

    Contributors:
        - i-Charlys: Original waiting room logic
        - Fshimi-Hawlk: Full widget migration, lobby_ prefix, CSC compliance
*/

#include "ui/waitingRoom.h"

#include "utils/globals.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/types.h"

static bool lobby_waitingRoomIsPanelVisible = false;
static bool lobby_waitingRoomIsInRoom = false;
static bool lobby_waitingRoomIsHost = false;
static s32  lobby_waitingRoomCurGameId = 0;
static s32  lobby_waitingRoomCurRoomId = 0;
static s32  lobby_waitingRoomPlayerCount = 1;
static s32  lobby_waitingRoomMaxPlayers = 4;

void lobby_initWaitingRoom(void) {
    lobby_waitingRoomIsPanelVisible = false;
    lobby_waitingRoomIsInRoom = false;
}

void lobby_showWaitingRoom(s32 gameId, s32 roomId, bool host) {
    lobby_waitingRoomIsInRoom = true;
    lobby_waitingRoomIsPanelVisible = true;
    lobby_waitingRoomCurGameId = gameId;
    lobby_waitingRoomCurRoomId = roomId;
    lobby_waitingRoomIsHost = host;
}

void lobby_updateWaitingRoomInfo(s32 players, s32 max, bool host) {
    lobby_waitingRoomPlayerCount = players;
    lobby_waitingRoomMaxPlayers = max;
    lobby_waitingRoomIsHost = host;
}

void lobby_setWaitingRoomPanelVisible(bool visible) {
    lobby_waitingRoomIsPanelVisible = visible;
}

void lobby_updateWaitingRoom(void) {
    if (!lobby_waitingRoomIsInRoom) return;

    if (lobby_waitingRoomIsPanelVisible) {
        // X button
        Rectangle btnX = { (float)GetScreenWidth()/2 + 170, (float)GetScreenHeight()/2 - 190, 30, 30 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnX)) {
            lobby_waitingRoomIsPanelVisible = false;
        }

        // Ready / Start button
        Rectangle btnAction = { (float)GetScreenWidth()/2 + 50, (float)GetScreenHeight()/2 + 150, 140, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnAction)) {
            if (lobby_waitingRoomIsHost) {
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_START_GAME, &h);
                u8 buf[64];
                memcpy(buf, &h, sizeof(h));
                s32 requestedPlayers = lobby_waitingRoomMaxPlayers;
                memcpy(buf + sizeof(h), &requestedPlayers, sizeof(s32));

                send(networkSocket, buf, sizeof(h) + sizeof(s32), 0);
                lobby_waitingRoomIsPanelVisible = false;
            } else {
                lobby_waitingRoomIsPanelVisible = false;
            }
        }

        // Quit button
        Rectangle btnQuit = { (float)GetScreenWidth()/2 - 190, (float)GetScreenHeight()/2 + 150, 140, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnQuit)) {
            lobby_waitingRoomIsInRoom = false;
            lobby_waitingRoomIsPanelVisible = false;
        }

        // Host max players selection
        if (lobby_waitingRoomIsHost) {
            if (IsKeyPressed(KEY_UP) && lobby_waitingRoomMaxPlayers < 4) lobby_waitingRoomMaxPlayers++;
            if (IsKeyPressed(KEY_DOWN) && lobby_waitingRoomMaxPlayers > 2) lobby_waitingRoomMaxPlayers--;
        }
    } else {
        // Click status text to reopen
        Rectangle statusRect = { (float)GetScreenWidth()/2 - 120, 10, 240, 35 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), statusRect)) {
            lobby_waitingRoomIsPanelVisible = true;
        }
    }
}

void lobby_drawWaitingRoom(void) {
    if (!lobby_waitingRoomIsInRoom) return;

    if (lobby_waitingRoomIsPanelVisible) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));
        
        Rectangle panel = { (float)GetScreenWidth()/2 - 200, (float)GetScreenHeight()/2 - 200, 400, 400 };
        DrawRectangleRec(panel, RAYWHITE);
        DrawRectangleLinesEx(panel, 2, DARKGRAY);
        
        DrawTextEx(
            lobby_fonts[FONT24], "WAITING ROOM",
            (Vector2){panel.x + 100, panel.y + 20}, 24, 0, BLACK
        );

        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("Game ID: %d | Room: %d", lobby_waitingRoomCurGameId, lobby_waitingRoomCurRoomId),
            (Vector2){panel.x + 20, panel.y + 50}, 18, 0, GRAY
        );

        // X Button
        Rectangle btnX = { panel.x + 370, panel.y + 10, 30, 30 };
        bool hoverX = CheckCollisionPointRec(GetMousePosition(), btnX);
        TextButton_St xBtn = {
            .bounds    = btnX,
            .state     = hoverX ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = RED,
            .roundness = 0.2f,
            .text      = "X",
            .textColor = WHITE
        };
        textButtonDraw(&xBtn, lobby_fonts[FONT24], 20.0f);

        // Player List
        DrawTextEx(lobby_fonts[FONT24], "Players:", (Vector2){panel.x + 20, panel.y + 80}, 20, 0, BLACK);
        DrawRectangle(panel.x + 20, panel.y + 105, 360, 220, LIGHTGRAY);

        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("- You (%s)", lobby_waitingRoomIsHost ? "Host" : "Guest"),
            (Vector2){panel.x + 30, panel.y + 120}, 18, 0, DARKGRAY
        );

        for (s32 i = 1; i < lobby_waitingRoomPlayerCount; i++) {
            DrawTextEx(
                lobby_fonts[FONT24],
                TextFormat("- Player %d", i + 1),
                (Vector2){panel.x + 30, panel.y + 120 + (i * 25)}, 18, 0, DARKGRAY
            );
        }

        // Quit button
        Rectangle btnQuit = { panel.x + 10, panel.y + 350, 140, 40 };
        bool hoverQuit = CheckCollisionPointRec(GetMousePosition(), btnQuit);
        TextButton_St quitBtn = {
            .bounds    = btnQuit,
            .state     = hoverQuit ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = ORANGE,
            .roundness = 0.2f,
            .text      = "QUIT",
            .textColor = WHITE
        };
        textButtonDraw(&quitBtn, lobby_fonts[FONT24], 18.0f);

        // Ready / Start button
        Rectangle btnAction = { panel.x + 250, panel.y + 350, 140, 40 };
        bool hoverAction = CheckCollisionPointRec(GetMousePosition(), btnAction);
        TextButton_St actionBtn = {
            .bounds    = btnAction,
            .state     = hoverAction ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = lobby_waitingRoomIsHost ? GOLD : GREEN,
            .roundness = 0.2f,
            .text      = lobby_waitingRoomIsHost ? "START" : "READY",
            .textColor = WHITE
        };
        textButtonDraw(&actionBtn, lobby_fonts[FONT24], 18.0f);

        if (lobby_waitingRoomIsHost) {
            DrawTextEx(
                lobby_fonts[FONT16],
                "↑↓ to change max players",
                (Vector2){panel.x + 100, panel.y + 330}, 16, 0, DARKGRAY
            );
            DrawTextEx(
                lobby_fonts[FONT24],
                TextFormat("Max: %d", lobby_waitingRoomMaxPlayers),
                (Vector2){panel.x + 300, panel.y + 80}, 18, 0, BLUE
            );
        }
    } else {
        // Status text when panel is hidden
        Rectangle statusRect = { (float)GetScreenWidth()/2 - 120, 10, 240, 35 };
        DrawRectangleRec(statusRect, Fade(RED, 0.8f));
        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("Waiting for players... (%d/%d)", lobby_waitingRoomPlayerCount, lobby_waitingRoomMaxPlayers),
            (Vector2){GetScreenWidth()/2.0f - 110, 20}, 18, 0, WHITE
        );
    }
}