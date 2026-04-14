/**
    @file ui/roomSelector.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @brief Room Selection UI for mini-games (Layer 2 of connection screen).

    Contributors:
        - i-Charlys:
            - Original room list logic
        - Fshimi-Hawlk:
            - Full widget migration, lobby_ prefix, CSC compliance, s32 usage
*/

#include "ui/roomSelector.h"

#include "raylib.h"
#include "utils/globals.h"

#include "sharedWidgets/button.h"
#include "sharedWidgets/types.h"

#define MAX_ROOMS_UI 16

static bool lobby_roomSelectorOpen = false;
static s32  lobby_currentGameId = 0;

static RoomInfo_St lobby_discoveredRooms[MAX_ROOMS_UI];
static s32     lobby_roomsCount = 0;

void lobby_initRoomSelector(void) {
    lobby_roomSelectorOpen = false;
    lobby_roomsCount = 0;
}

void lobby_openRoomSelector(s32 gameId) {
    printf("[UI] Opening Room Selector for Game ID: %d\n", gameId);
    lobby_roomSelectorOpen = true;
    lobby_currentGameId = gameId;
    lobby_roomsCount = 0;

    // Send query to server
    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_ROOM_QUERY, &h);
    h.senderId = htons((u16)lobby_game.clientId);

    u8 buf[64];
    memcpy(buf, &h, sizeof(h));
    buf[sizeof(h)] = (gameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)gameId;

    printf("[NET] Sending ROOM_QUERY\n");
    send(networkSocket, buf, sizeof(h) + 1, 0);
}

void lobby_closeRoomSelector(void) {
    if (lobby_roomSelectorOpen) printf("[UI] Closing Room Selector\n");
    lobby_roomSelectorOpen = false;
}

void lobby_handleRoomList(const void* data, s32 count) {
    printf("[UI] Received %d rooms from server\n", count);
    if (count > MAX_ROOMS_UI) count = MAX_ROOMS_UI;
    memcpy(lobby_discoveredRooms, data, (size_t)count * sizeof(RoomInfo_St));
    lobby_roomsCount = count;

    // Convert network to host byte order
    for (s32 i = 0; i < lobby_roomsCount; i++) {
        lobby_discoveredRooms[i].id = ntohs(lobby_discoveredRooms[i].id);
        lobby_discoveredRooms[i].playerCount = ntohs(lobby_discoveredRooms[i].playerCount);
        printf("[UI] Room %d: '%s' by %s (%d players)\n", 
               lobby_discoveredRooms[i].id, lobby_discoveredRooms[i].name, 
               lobby_discoveredRooms[i].creator, lobby_discoveredRooms[i].playerCount);
    }
}

bool lobby_updateRoomSelector(void) {
    if (!lobby_roomSelectorOpen) return false;

    if (IsKeyPressed(KEY_ESCAPE) && lobby_currentGameId != -1) {
        lobby_closeRoomSelector();
        return false;
    }

    extern LobbyGame_St lobby_game;

    if (IsKeyPressed(KEY_ENTER)) {
        if (lobby_roomsCount > 0) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.senderId = htons((u16)lobby_game.clientId);

            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (lobby_currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)lobby_currentGameId;
            buf[sizeof(h) + 1] = (s8)lobby_discoveredRooms[0].id;

            send(networkSocket, buf, sizeof(h) + 2, 0);
        } else {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.senderId = htons((u16)lobby_game.clientId);

            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (lobby_currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)lobby_currentGameId;
            buf[sizeof(h) + 1] = (s8)-1;

            send(networkSocket, buf, sizeof(h) + 2, 0);
        }
        lobby_closeRoomSelector();
        return true;
    }

    // New Room button (only when selecting a specific mini-game)
    if (lobby_currentGameId != -1) {
        Rectangle btnNew = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 150, 200, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnNew)) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.senderId = htons((u16)lobby_game.clientId);

            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (u8)lobby_currentGameId;
            buf[sizeof(h) + 1] = (s8)-1;

            send(networkSocket, buf, sizeof(h) + 2, 0);
            lobby_closeRoomSelector();
            return true;
        }
    }

    // Join existing rooms
    for (s32 i = 0; i < lobby_roomsCount; i++) {
        Rectangle r = { (float)GetScreenWidth()/2 - 150, (float)GetScreenHeight()/2 - 100 + (i * 50), 300, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r)) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.senderId = htons((u16)lobby_game.clientId);

            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (lobby_currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)lobby_currentGameId;
            buf[sizeof(h) + 1] = (s8)lobby_discoveredRooms[i].id;

            send(networkSocket, buf, sizeof(h) + 2, 0);
            lobby_closeRoomSelector();
            return true;
        }
    }

    return false;
}

void lobby_drawRoomSelector(void) {
    if (!lobby_roomSelectorOpen) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
    
    Rectangle panel = { (float)GetScreenWidth()/2 - 200, (float)GetScreenHeight()/2 - 210, 400, 440 };
    DrawRectangleRec(panel, RAYWHITE);
    DrawRectangleLinesEx(panel, 2, DARKGRAY);
    
    // Title
    const char* title = (lobby_currentGameId == -1) ? "AVAILABLE ROOMS" : "ROOM SELECTION";
    Vector2 titleSize = MeasureTextEx(lobby_fonts[FONT24], title, 24, 0);
    DrawTextEx(
        lobby_fonts[FONT24], title,
        (Vector2){panel.x + (panel.width - titleSize.x) / 2.0f, panel.y + 20},
        24, 0, BLACK
    );

    // Column headers (placed where the old "Enter Lobby" button was)
    DrawTextEx(lobby_fonts[FONT24], "NAME",     (Vector2){panel.x + 65, panel.y + 72}, 20, 0, DARKGRAY);
    DrawTextEx(lobby_fonts[FONT24], "CREATOR",  (Vector2){panel.x + 215, panel.y + 72}, 20, 0, DARKGRAY);
    DrawTextEx(lobby_fonts[FONT24], "PLAYERS",  (Vector2){panel.x + 305, panel.y + 72}, 20, 0, DARKGRAY);

    if (lobby_roomsCount == 0) {
        DrawTextEx(
            lobby_fonts[FONT24], "No games in progress.",
            (Vector2){panel.x + 80, panel.y + 120}, 20, 0, GRAY
        );
    }

    // Room list - raised to use freed space
    for (s32 i = 0; i < lobby_roomsCount; i++) {
        Rectangle r = { panel.x + 50, panel.y + 105 + (i * 50), 300, 40 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);

        TextButton_St btn = {
            .bounds    = r,
            .state     = hover ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = hover ? LIGHTGRAY : GRAY,
            .roundness = 0.2f,
            .text      = "",                    // we draw custom text manually
            .textColor = WHITE
        };
        textButtonDraw(&btn, lobby_fonts[FONT24], 20.0f);

        // Room name (left)
        DrawTextEx(
            lobby_fonts[FONT24],
            lobby_discoveredRooms[i].name,
            (Vector2){r.x + 15, r.y + 11},
            20, 0, BLACK
        );

        // Creator (right side)
        DrawTextEx(
            lobby_fonts[FONT24],
            lobby_discoveredRooms[i].creator,
            (Vector2){r.x + 195, r.y + 13},
            18, 0, DARKGRAY
        );

        // Player count (far right)
        DrawTextEx(
            lobby_fonts[FONT24],
            TextFormat("%d", lobby_discoveredRooms[i].playerCount),
            (Vector2){r.x + r.width - 20, r.y + 13},
            18, 0, BLUE
        );
    }

    // Create new room button (only for specific mini-games)
    if (lobby_currentGameId != -1) {
        Rectangle btnNew = { panel.x + 100, panel.y + 360, 200, 40 };
        bool hoverNew = CheckCollisionPointRec(GetMousePosition(), btnNew);

        TextButton_St btn = {
            .bounds    = btnNew,
            .state     = hoverNew ? WIDGET_STATE_HOVER : WIDGET_STATE_NORMAL,
            .baseColor = hoverNew ? LIME : GREEN,
            .roundness = 0.2f,
            .text      = "CREATE ROOM (+)",
            .textColor = WHITE
        };
        textButtonDraw(&btn, lobby_fonts[FONT24], 18.0f);
    }
    
    if (lobby_currentGameId != -1)
        DrawTextEx(lobby_fonts[FONT16], "ESC to close", (Vector2){panel.x + 140, panel.y + 410}, 16, 0, DARKGRAY);
}