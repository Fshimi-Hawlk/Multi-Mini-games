/**
    @file ui/connectionScreen.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-08
    @date 2026-04-11
    @brief Modern connection screen using the enhanced reusable widget system.
*/

#include "ui/roomSelector.h"

#define MAX_ROOMS_UI 16

static bool isOpen = false;
static int currentGameId = 0;
static RoomInfo_St discoveredRooms[MAX_ROOMS_UI];
static int roomsCount = 0;

void initRoomSelector(void) {
    isOpen = false;
    roomsCount = 0;
}

void openRoomSelector(int gameId) {
    printf("[UI] Opening Room Selector for Game ID: %d\n", gameId);
    isOpen = true;
    currentGameId = gameId;
    roomsCount = 0;

    // Send query to server
    extern LobbyGame_St lobby_game;
    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_ROOM_QUERY, &h);
    h.sender_id = htons((u16)lobby_game.clientId);
    u8 buf[64];
    memcpy(buf, &h, sizeof(h));
    
    // gameId -1 means ALL rooms (for Layer 2)
    buf[sizeof(h)] = (gameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)gameId; 

    printf("[NET] Sending ROOM_QUERY\n");
    send(networkSocket, buf, sizeof(h) + 1, 0);
}

void closeRoomSelector(void) {
    if (isOpen) printf("[UI] Closing Room Selector\n");
    isOpen = false;
}

void handleRoomList(const void* data, int count) {
    printf("[UI] Received %d rooms from server\n", count);
    if (count > MAX_ROOMS_UI) count = MAX_ROOMS_UI;
    memcpy(discoveredRooms, data, count * sizeof(RoomInfo_St));
    roomsCount = count;

    // Convert network to host byte order
    for (int i = 0; i < roomsCount; i++) {
        discoveredRooms[i].id = ntohs(discoveredRooms[i].id);
        discoveredRooms[i].playerCount = ntohs(discoveredRooms[i].playerCount);
        printf("[UI] Room %d: '%s' by %s (%d players)\n", 
               discoveredRooms[i].id, discoveredRooms[i].name, discoveredRooms[i].creator, discoveredRooms[i].playerCount);
    }
}

bool updateRoomSelector(void) {
    if (!isOpen) return false;

    if (IsKeyPressed(KEY_ESCAPE) && currentGameId != -1) {
        closeRoomSelector();
        return false;
    }

    extern LobbyGame_St lobby_game;
    if (IsKeyPressed(KEY_ENTER)) {
        // If there are rooms, join the first one
        if (roomsCount > 0) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.sender_id = htons((u16)lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)currentGameId;
            buf[sizeof(h) + 1] = (s8)discoveredRooms[0].id;
            send(networkSocket, buf, sizeof(h) + 2, 0);
        } else {
            // Create a new room
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.sender_id = htons((u16)lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)currentGameId;
            buf[sizeof(h) + 1] = (s8)-1;
            send(networkSocket, buf, sizeof(h) + 2, 0);
        }
        closeRoomSelector();
        return true;
    }

    // Layer 2 specific: Join Lobby button
    if (currentGameId == -1) {
        Rectangle btnLobby = { (float)GetScreenWidth()/2 - 150, (float)GetScreenHeight()/2 - 160, 300, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnLobby)) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.sender_id = htons((u16)lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            buf[sizeof(h)] = (u8)MINI_GAME_ID_LOBBY;
            buf[sizeof(h) + 1] = (s8)0;

            send(networkSocket, buf, sizeof(h) + 2, 0);
            
            closeRoomSelector();
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            return true;
        }
    }

    // Check for "New Room" (+) — désactivé en mode global (currentGameId == -1)
    // car envoyer MINI_GAME_ID_LOBBY avec targetRoomId=-1 est rejeté par le serveur
    Rectangle btnNew = { (float)GetScreenWidth()/2 - 100, (float)GetScreenHeight()/2 + 150, 200, 40 };
    if (currentGameId != -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), btnNew)) {
        RUDPHeader_St h;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
        h.sender_id = htons((u16)lobby_game.clientId);
        u8 buf[64];
        memcpy(buf, &h, sizeof(h));
        // If global, we create a Lobby by default (to enter the world)
        buf[sizeof(h)] = (currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)currentGameId;
        buf[sizeof(h) + 1] = (s8)-1;

        send(networkSocket, buf, sizeof(h) + 2, 0);
        closeRoomSelector();
        return true;
    }

    // Check for joining existing rooms
    for (int i = 0; i < roomsCount; i++) {
        Rectangle r = { (float)GetScreenWidth()/2 - 150, (float)GetScreenHeight()/2 - 100 + (i * 50), 300, 40 };
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r)) {
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
            h.sender_id = htons((u16)lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            // We use the gameId from the discovered room if global
            buf[sizeof(h)] = (currentGameId == -1) ? (u8)MINI_GAME_ID_LOBBY : (u8)currentGameId; // Need to store gameId in RoomInfo_St for full global support
            buf[sizeof(h) + 1] = (s8)discoveredRooms[i].id;

            send(networkSocket, buf, sizeof(h) + 2, 0);
            closeRoomSelector();
            return true;
        }
    }

    return false;
}

void drawRoomSelector(void) {
    if (!isOpen) return;

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
    
    Rectangle panel = { (float)GetScreenWidth()/2 - 200, (float)GetScreenHeight()/2 - 210, 400, 440 };
    DrawRectangleRec(panel, RAYWHITE);
    DrawRectangleLinesEx(panel, 2, DARKGRAY);
    
    DrawText("SÉLECTION DE SALON", (int)panel.x + 100, (int)panel.y + 20, 20, BLACK);

    if (currentGameId == -1) {
        Rectangle btnLobby = { panel.x + 50, panel.y + 60, 300, 40 };
        bool hoverL = CheckCollisionPointRec(GetMousePosition(), btnLobby);
        DrawRectangleRec(btnLobby, hoverL ? SKYBLUE : BLUE);
        DrawText("ENTRER DANS LE LOBBY", (int)btnLobby.x + 40, (int)btnLobby.y + 10, 18, WHITE);
    }

    if (roomsCount == 0) {
        DrawText("Aucune partie en cours.", (int)panel.x + 80, (int)panel.y + 120, 18, GRAY);
    }

    for (int i = 0; i < roomsCount; i++) {
        Rectangle r = { panel.x + 50, panel.y + 110 + (i * 50), 300, 40 };
        bool hover = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRec(r, hover ? LIGHTGRAY : GRAY);
        DrawRectangleLinesEx(r, 1, BLACK);
        DrawText(TextFormat("%s", discoveredRooms[i].name), (int)r.x + 10, (int)r.y + 5, 16, WHITE);
        DrawText(TextFormat("Par: %s - %d J", discoveredRooms[i].creator, discoveredRooms[i].playerCount), (int)r.x + 10, (int)r.y + 22, 12, LIGHTGRAY);
    }

    if (currentGameId != -1) {
        Rectangle btnNew = { panel.x + 100, panel.y + 360, 200, 40 };
        bool hoverNew = CheckCollisionPointRec(GetMousePosition(), btnNew);
        DrawRectangleRec(btnNew, hoverNew ? LIME : GREEN);
        DrawText("CRÉER SALON (+)", (int)btnNew.x + 30, (int)btnNew.y + 10, 18, WHITE);
    }
    
    if (currentGameId != -1)
        DrawText("ESC pour fermer", (int)panel.x + 140, (int)panel.y + 410, 15, DARKGRAY);
}
