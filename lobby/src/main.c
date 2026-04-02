/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-02-08
    @date 2026-03-30
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
*/

#include "core/game.h"
#include "ui/connection_screen.h"
#include "ui/menus.h"
#include "core/chat.h"
#include "utils/globals.h"
#include "firstparty/progress.h"
#include "firstparty/leaderboard.h"
#include "setups/app.h"
#include "systemSettings.h"
#include "APIs/generalAPI.h"
#include "editor/editor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

s32 networkSocket = -1;
RUDPConnection_St serverConnection;

static MiniGame_Et currentMiniGameID = MINI_GAME_LOBBY;
static GameClientInterface_St* currentMiniGame = NULL;

extern GameClientInterface_St lobbyClientInterface;
extern GameClientInterface_St kingForFourClientInterface;
extern GameClientInterface_St bingoClientInterface;
extern GameClientInterface_St ChessClientModule;
extern GameClientInterface_St RubikClientModule;

// Pointers to the mini-game client interfaces
static GameClientInterface_St* miniGameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY] = &lobbyClientInterface,
    [MINI_GAME_KFF]   = &kingForFourClientInterface,
    [MINI_GAME_BINGO] = &bingoClientInterface,
    [MINI_GAME_CHESS] = &ChessClientModule,
    [MINI_GAME_CUBE]  = &RubikClientModule,
};

static bool server_spawned = false;

void kill_server(void) {
    if (server_spawned) {
        log_info("Arrêt du serveur...");
#ifdef _WIN32
        system("taskkill /F /IM server.exe > nul 2>&1");
#else
        system("killall server 2>/dev/null");
#endif
        server_spawned = false;
    }
}

void force_kill_server(void) {
#ifdef _WIN32
    system("taskkill /F /IM server.exe > nul 2>&1");
#else
    system("killall server 2>/dev/null");
#endif
}

void spawn_server(void) {
    if (server_spawned) return;
    log_info("Lancement du serveur...");
#ifdef _WIN32
    system("start /B reseau\\build\\bin\\server.exe");
#else
    system("./reseau/build/bin/server &");
#endif
    server_spawned = true;
}

Error_Et switchMinigame(const MiniGame_Et nextMiniGame) {
    if (nextMiniGame >= __miniGameCount) return ERROR_INVALID_ENUM_VAL;
    
    GameClientInterface_St* interface = miniGameInterfaces[nextMiniGame];
    if (interface == NULL) {
        log_error("Received idx for non-integrated game: %d", nextMiniGame);
        return ERROR_INVALID_ENUM_VAL;
    }

    interface->init();
    currentMiniGame = interface;
    currentMiniGameID = nextMiniGame;
    
    if (nextMiniGame == MINI_GAME_LOBBY) lobby_game.currentState = GAME_STATE_GAMEPLAY;
    else lobby_game.currentState = GAME_STATE_INGAME;

    return OK;
}

void switch_minigame(u8 game_id) {
    switchMinigame((MiniGame_Et)game_id);
}

void ensureSocketExists(void) {
    if (networkSocket != -1) return;
    networkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (networkSocket >= 0) {
        int brd = 1;
        setsockopt(networkSocket, SOL_SOCKET, SO_BROADCAST, &brd, sizeof(brd));
        fcntl(networkSocket, F_SETFL, O_NONBLOCK);
    }
}

void discoverServers(void) {
    ensureSocketExists();
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr.s_addr = INADDR_BROADCAST
    };
    RUDPHeader_St q = { .action = ACTION_CODE_LOBBY_ROOM_QUERY };
    sendto(networkSocket, &q, sizeof(RUDPHeader_St), 0, (struct sockaddr*) &addr, sizeof(addr));
}

void initNetwork(const char* targetIp) {
    ensureSocketExists();
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(SERVER_PORT) };
    inet_pton(AF_INET, targetIp, &addr.sin_addr);
    if (connect(networkSocket, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        log_error("Couldn't connect to %s:%d", targetIp, SERVER_PORT);
    }
    rudpInitConnection(&serverConnection);
    
    GameTLVHeader_St tlv = { .game_id = MINI_GAME_LOBBY, .action = ACTION_CODE_JOIN_GAME, .length = 0 };
    u8 buffer[128];
    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
    memcpy(buffer, &h, sizeof(h)); memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    send(networkSocket, buffer, sizeof(h) + sizeof(tlv), 0);
}

void receiveNetworkData(void) {
    if (networkSocket == -1) return;
    u8 buffer[2048];
    struct sockaddr_in from; socklen_t len;
    while (1) {
        len = sizeof(from);
        ssize_t r = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &len);
        if (r < 0) break;
        if (r < (ssize_t) sizeof(RUDPHeader_St)) break;

        RUDPHeader_St* h = (RUDPHeader_St*) buffer;

        if (h->action == ACTION_CODE_LOBBY_ROOM_INFO) {
            char ip_str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &from.sin_addr, ip_str, sizeof(ip_str));
            addDiscoveredRoom(ip_str, (char*) (buffer + sizeof(RUDPHeader_St)));
            continue;
        }

        if (!rudpProcessIncoming(&serverConnection, h)) continue;

        if (h->action == ACTION_CODE_LOBBY_SWITCH_GAME) {
            u8 targetGameId = *(u8*) (buffer + sizeof(RUDPHeader_St));
            switchMinigame((MiniGame_Et)targetGameId);
            continue;
        }

        if (h->action == ACTION_CODE_GAME_DATA) {
            GameTLVHeader_St* tlv = (GameTLVHeader_St*) (buffer + sizeof(RUDPHeader_St));
            void* payload = (u8*) tlv + sizeof(GameTLVHeader_St);
            if (tlv->game_id < __miniGameCount && miniGameInterfaces[tlv->game_id]) {
                miniGameInterfaces[tlv->game_id]->on_data(ntohs(h->sender_id), tlv->action, payload, tlv->length);
            }
            continue;
        }
    }
}

int main(void) {
    force_kill_server();
    g_progress = LoadProgress();
    
    if (initApp() != OK) return 1;

    InitMenus();
    miniGameInterfaces[MINI_GAME_LOBBY]->init();
    currentMiniGame = miniGameInterfaces[MINI_GAME_LOBBY];
    initConnectionScreen();

    static bool switch_sent = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        
        receiveNetworkData();

        if (IsWindowResized()) {
            systemSettings.video.width  = GetScreenWidth();
            systemSettings.video.height = GetScreenHeight();
        }

        UpdateMenu();

        switch (lobby_game.currentState) {
            case GAME_STATE_CONNECTION:
                static f32 disc_timer = 0;
                disc_timer += dt;
                if (disc_timer > 2.0f) { discoverServers(); disc_timer = 0; }
                if (updateConnectionScreen()) {
                    initNetwork(getEnteredIP());
                    strncpy(lobby_game.player.name, getEnteredPseudo(), 31);
                    lobby_game.currentState = GAME_STATE_GAMEPLAY;
                }
                BeginDrawing();
                ClearBackground(RAYWHITE);
                drawConnectionScreen();
                EndDrawing();
                break;

            case GAME_STATE_GAMEPLAY:
            case GAME_STATE_INGAME:
                if (currentMiniGameID == MINI_GAME_LOBBY && g_currentMenu == MENU_NONE && !lobby_game.editorMode) {
                    MiniGame_Et triggerID = MINI_GAME_LOBBY;
                    for (int i = 1; i < __miniGameCount; i++) {
                        if (CheckCollisionCircleRec(lobby_game.player.position, lobby_game.player.radius, gameInteractionZones[i].hitbox)) {
                            triggerID = i;
                            break;
                        }
                    }

                    if (triggerID != MINI_GAME_LOBBY) {
                        const char* gname = gameInteractionZones[triggerID].name;
                        DrawText(TextFormat("APPUYEZ SUR ENTRÉE POUR JOUER : %s", gname), 
                                 GetScreenWidth()/2 - MeasureText(TextFormat("APPUYEZ SUR ENTRÉE POUR JOUER : %s", gname), 20)/2, 
                                 GetScreenHeight() - 100, 20, GOLD);
                        
                        if (IsKeyPressed(KEY_ENTER) && !switch_sent) {
                            RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_LOBBY_SWITCH_GAME, &h);
                            u8 buf[64]; memcpy(buf, &h, sizeof(h)); buf[sizeof(h)] = (u8)triggerID;
                            send(networkSocket, buf, sizeof(h) + 1, 0);
                            switch_sent = true;
                        }
                    } else {
                        switch_sent = false;
                    }
                }

                if (currentMiniGame) {
                    if (g_currentMenu == MENU_NONE) currentMiniGame->update(dt);
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    currentMiniGame->draw();
                    if (g_currentMenu != MENU_NONE) DrawMenu();
                    DrawFPS(10, 10);
                    EndDrawing();
                }
                break;
            default: break;
        }
    }

    SaveProgress(&g_progress);
    kill_server();
    freeApp();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
#define SYSTEM_SETTINGS_IMPLEMENTATION
#include "systemSettings.h"
#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"
