/**
    @file main.c
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-02-08
    @date 2026-04-02
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
*/

#include "core/game.h"
#include "ui/connection_screen.h"
#include "ui/room_selector.h"
#include "ui/waiting_room.h"
#include "ui/menus.h"
#include "core/chat.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "firstparty/progress.h"
#include "firstparty/leaderboard.h"
#include "setups/app.h"
#include "systemSettings.h"
#include "APIs/generalAPI.h"
#include "editor/editor.h"
#include "ui/game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
#include <signal.h>
#include <sys/types.h>
#endif

s32 networkSocket = -1;
RUDPConnection_St serverConnection; 

static MiniGame_Et currentMiniGameID = MINI_GAME_LOBBY;
static GameClientInterface_St* currentMiniGame = NULL;

extern GameClientInterface_St lobbyClientInterface;
extern GameClientInterface_St kingForFourClientInterface;
extern GameClientInterface_St bingoClientInterface;
extern GameClientInterface_St ChessClientModule;
extern GameClientInterface_St RubikClientModule;
extern GameClientInterface_St editorClientInterface;

// Pointers to the mini-game client interfaces
static GameClientInterface_St* miniGameInterfaces[__miniGameCount] = {
    [MINI_GAME_LOBBY]  = &lobbyClientInterface,
    [MINI_GAME_KFF]    = &kingForFourClientInterface,
    [MINI_GAME_BINGO]  = &bingoClientInterface,
    [MINI_GAME_CHESS]  = &ChessClientModule,
    [MINI_GAME_CUBE]   = &RubikClientModule,
    [MINI_GAME_EDITOR] = &editorClientInterface,
};

static bool server_spawned = false;

#ifndef _WIN32
static pid_t server_pid = -1;
#endif

void kill_server(void) {
    if (server_spawned) {
        log_info("Arrêt du serveur local...");
#ifdef _WIN32
        system("taskkill /F /IM server.exe > nul 2>&1");
#else
        if (server_pid > 0) {
            kill(server_pid, SIGTERM);
            server_pid = -1;
        } else {
            system("killall server 2>/dev/null");
        }
#endif
        server_spawned = false;
    }
}

void discoverServers(void) {
    if (networkSocket < 0) {
        networkSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (networkSocket < 0) return;
        int broadcastEnable = 1;
        setsockopt(networkSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
        int flags = fcntl(networkSocket, F_GETFL, 0);
        fcntl(networkSocket, F_SETFL, flags | O_NONBLOCK);
        log_info("[NET] Discovery socket initialized");
    }

    struct sockaddr_in broadcastAddr;
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(8080);
    broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    // Localhost specifically for local tests
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(8080);
    localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    RUDPHeader_St h_disc = { .action = ACTION_CODE_DISCOVERY_QUERY, .sequence = 0, .ack = 0, .ack_bitfield = 0, .sender_id = 0 };
    sendto(networkSocket, &h_disc, sizeof(h_disc), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
    sendto(networkSocket, &h_disc, sizeof(h_disc), 0, (struct sockaddr*)&localAddr, sizeof(localAddr));

    log_debug("Discovery queries sent...");
}

void initNetwork(const char* ip, const char* pseudo) {
    if (networkSocket != -1) close(networkSocket);

    networkSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (networkSocket < 0) {
        log_error("Failed to create socket");
        return;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    rudpInitConnection(&serverConnection);

    // Connect socket to allow using send() instead of sendto() for gameplay
    if (connect(networkSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        log_error("Failed to connect socket");
    }

    // Send Join Game with Pseudo
    RUDPHeader_St h;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_JOIN_GAME, &h);
    u8 buf[128];
    memcpy(buf, &h, sizeof(h));
    strncpy((char*)buf + sizeof(h), pseudo, 31);

    send(networkSocket, buf, sizeof(h) + strlen(pseudo) + 1, 0);
    log_info("Sent JOIN_GAME for player '%s'", pseudo);

    int flags = fcntl(networkSocket, F_GETFL, 0);
    fcntl(networkSocket, F_SETFL, flags | O_NONBLOCK);

    log_info("Network initialized towards %s:8080", ip);
}

void receiveNetworkData(void) {
    if (networkSocket < 0) return;

    u8 buffer[2048];
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);

    while (1) {
        ssize_t bytesRead = recvfrom(networkSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&fromAddr, &fromLen);
        
        if (bytesRead < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                log_error("[NET] recvfrom error: %s", strerror(errno));
            }
            break;
        }

        if (bytesRead >= (ssize_t)sizeof(RUDPHeader_St)) {
            RUDPHeader_St header;
            memcpy(&header, buffer, sizeof(RUDPHeader_St));
            
            // Handle discovery outside of RUDP state machine
            if (header.action == ACTION_CODE_DISCOVERY_INFO && ntohs(header.sender_id) == 999) {
                char* info = (char*)buffer + sizeof(RUDPHeader_St);
                log_info("[DISCOVERY] Found Server at %s: %s", inet_ntoa(fromAddr.sin_addr), info);
                extern void addDiscoveredRoom(const char* ip, const char* name);
                addDiscoveredRoom(inet_ntoa(fromAddr.sin_addr), info);
                continue;
            }

            if (rudpProcessIncoming(&serverConnection, &header)) {
                u8* payload = buffer + sizeof(RUDPHeader_St);
                u16 payloadLen = (u16)(bytesRead - sizeof(RUDPHeader_St));
                u16 senderId = ntohs(header.sender_id);

                if (header.action == ACTION_CODE_JOIN_ERROR) {
                    log_error("[NET] Join rejected by server: %s", (char*)payload);
                    setConnectionError((char*)payload);
                    lobby_game.currentState = GAME_STATE_CONNECTION;
                    // Reset network state if needed
                    rudpInitConnection(&serverConnection);
                }
                else if (header.action == ACTION_CODE_LOBBY_ROOM_INFO) {
                    extern void handleRoomList(const void* data, int count);
                    handleRoomList(payload, payloadLen / sizeof(RoomInfo_St));
                }
                else if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                    if (payloadLen >= 2) {
                        u8 nextGame = payload[0];
                        u8 roomId = payload[1];
                        log_info("[NET] Server confirmed switch to Game %d, Room %d", nextGame, roomId);
                        
                        if (nextGame < __miniGameCount) {
                            if (currentMiniGame && currentMiniGame->destroy) {
                                currentMiniGame->destroy();
                            }
                            currentMiniGameID = (MiniGame_Et)nextGame;
                            currentMiniGame = miniGameInterfaces[currentMiniGameID];
                            if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();
                            
                            closeRoomSelector();
                            lobby_game.currentState = (nextGame == MINI_GAME_LOBBY) ? GAME_STATE_GAMEPLAY : GAME_STATE_INGAME;
                            if (nextGame == MINI_GAME_LOBBY) initWaitingRoom();
                        }
                    }
                }
                else if (header.action == ACTION_CODE_GAME_DATA) {
                    if (payloadLen >= sizeof(GameTLVHeader_St)) {
                        GameTLVHeader_St tlv;
                        memcpy(&tlv, payload, sizeof(tlv));
                        u16 tlv_data_len = ntohs(tlv.length);
                        
                        if (tlv_data_len <= payloadLen - sizeof(GameTLVHeader_St)) {
                            if (tlv.game_id < __miniGameCount && miniGameInterfaces[tlv.game_id]) {
                                miniGameInterfaces[tlv.game_id]->on_data(senderId, tlv.action, payload + sizeof(tlv), tlv_data_len);
                            }
                        }
                    }
                } 
                else if (currentMiniGame && currentMiniGame->on_data) {
                    if (header.action < firstAvailableActionCode || header.action == ACTION_CODE_JOIN_ACK) {
                        currentMiniGame->on_data(senderId, header.action, payload, payloadLen);
                    }
                }
            }
        }
    }
}

void spawn_server(void) {
    if (!server_spawned) {
        log_info("Lancement du serveur local...");
#ifdef _WIN32
        system("start /B server.exe");
        server_spawned = true;
#else
        server_pid = fork();
        if (server_pid == 0) {
            // Child process
            execl("./build/bin/server", "server", (char*)NULL);
            perror("execl failed");
            exit(1);
        } else if (server_pid > 0) {
            server_spawned = true;
        } else {
            log_error("Fork failed to spawn server");
        }
#endif
    }
}

// Backup of lobby terrains before entering editor (so editor changes don't persist in lobby)
static TerrainVec_St lobbyTerrainBackup = {0};

void switch_minigame(u8 game_id) {
    if (game_id < __miniGameCount && miniGameInterfaces[game_id]) {
        // Notify server that we are leaving the current minigame room (if any)
        if (currentMiniGameID != MINI_GAME_LOBBY && currentMiniGameID != MINI_GAME_EDITOR && game_id == MINI_GAME_LOBBY) {
            GameTLVHeader_St tlv = { .game_id = currentMiniGameID, .action = ACTION_CODE_QUIT_GAME, .length = 0 };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.sender_id = htons((u16)lobby_game.id);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
            send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
        }

        // Leaving editor → restore lobby terrains
        if (currentMiniGameID == MINI_GAME_EDITOR && game_id != MINI_GAME_EDITOR) {
            da_clear(&terrains);
            if (lobbyTerrainBackup.count > 0)
                da_append_many(&terrains, lobbyTerrainBackup.items, lobbyTerrainBackup.count);
        }
        // Entering editor → backup lobby terrains
        if (game_id == MINI_GAME_EDITOR) {
            da_clear(&lobbyTerrainBackup);
            if (terrains.count > 0)
                da_append_many(&lobbyTerrainBackup, terrains.items, terrains.count);
        }

        if (currentMiniGame && currentMiniGame->destroy) {
            currentMiniGame->destroy();
        }
        currentMiniGameID = (MiniGame_Et)game_id;
        currentMiniGame = miniGameInterfaces[currentMiniGameID];
        if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();

        // Ensure UI state is cleared when switching
        lobby_game.currentState = GAME_STATE_GAMEPLAY;
        closeRoomSelector();

        if (game_id == MINI_GAME_LOBBY) {
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            lobby_game.editorMode = false;
            initWaitingRoom(); // Réinitialise l'overlay pour éviter qu'il reste visible au retour lobby
        } else {
            lobby_game.currentState = GAME_STATE_INGAME;
            if (game_id == MINI_GAME_EDITOR) lobby_game.editorMode = true;
        }
        log_info("Switched to mini-game ID: %d", game_id);
    }
}

int main(void) {
    g_progress = LoadProgress();
    
    if (initApp() != OK) return 1;

    InitMenus();
    miniGameInterfaces[MINI_GAME_LOBBY]->init();
    currentMiniGame = miniGameInterfaces[MINI_GAME_LOBBY];
    initConnectionScreen();
    // Use the default pseudo from the connection screen as the initial player name
    strncpy(lobby_game.player.name, getEnteredPseudo(), 31);
    initRoomSelector();
    initWaitingRoom();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        
        receiveNetworkData();

        if (IsWindowResized()) {
            systemSettings.video.width  = GetScreenWidth();
            systemSettings.video.height = GetScreenHeight();
        }

        UpdateMenu();
        bool selectorActive = updateRoomSelector();
        // L'overlay "salle d'attente" du lobby n'est actif qu'en lobby :
        // chaque module de jeu gère sa propre phase pré-partie.
        if (currentMiniGameID == MINI_GAME_LOBBY) updateWaitingRoom();

        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (g_currentMenu != MENU_NONE) {
            DrawMenu();
        } 
        else if (lobby_game.currentState == GAME_STATE_CONNECTION) {
            static f32 disc_timer = 0;
            disc_timer += dt;
            if (disc_timer > 2.0f) { discoverServers(); disc_timer = 0; }
            
            if (updateConnectionScreen()) {
                const char* pseudo = getEnteredPseudo();
                initNetwork(getEnteredIP(), pseudo);
                strncpy(lobby_game.player.name, pseudo, 31);
                lobby_game.currentState = GAME_STATE_ROOM_LIST;
                openRoomSelector(-1); // Open global room list
            }
            drawConnectionScreen();
        } 
        else if (lobby_game.currentState == GAME_STATE_ROOM_LIST) {
            drawRoomSelector();
        }
        else {
            // Gameplay loop
            static MiniGame_Et lastTriggerID = MINI_GAME_LOBBY;

            if (currentMiniGameID == MINI_GAME_LOBBY && !lobby_game.editorMode && !selectorActive) {
                MiniGame_Et triggerID = MINI_GAME_LOBBY;
                for (int i = 1; i < __miniGameCount; i++) {
                    if (gameInteractionZones[i].hitbox.width > 0 &&
                        CheckCollisionCircleRec(lobby_game.player.position, lobby_game.player.radius, gameInteractionZones[i].hitbox)) {
                        triggerID = i;
                        break;
                    }
                }

                if (triggerID != MINI_GAME_LOBBY) {
                    if (lastTriggerID != triggerID) {
                        log_info("[ZONE] Entered Zone for Game ID %d", triggerID);
                        lastTriggerID = triggerID;
                    }

                    const char* gname = gameInteractionZones[triggerID].name;
                    DrawText(TextFormat("APPUYEZ SUR ENTRÉE POUR : %s", gname), 
                             GetScreenWidth()/2 - MeasureText(TextFormat("APPUYEZ SUR ENTRÉE POUR : %s", gname), 20)/2, 
                             GetScreenHeight() - 100, 20, GREEN);

                    if (IsKeyPressed(KEY_ENTER)) {
                        // Solo mode (no server) or editor: switch directly without room selector
                        if (triggerID == MINI_GAME_EDITOR || networkSocket < 0) {
                            switch_minigame(triggerID);
                        } else {
                            openRoomSelector(triggerID);
                        }
                    }
                } else {
                    if (lastTriggerID != MINI_GAME_LOBBY) {
                        closeRoomSelector();
                        lastTriggerID = MINI_GAME_LOBBY;
                    }
                }
            }

            if (currentMiniGame) {
                currentMiniGame->update(dt);
                drawLobbyBackground();
                currentMiniGame->draw();
            }
            
            drawRoomSelector();
            if (currentMiniGameID == MINI_GAME_LOBBY) drawWaitingRoom();
        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    SaveProgress(&g_progress);

    if (networkSocket != -1) {
        RUDPHeader_St h;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_QUIT_GAME, &h);
        h.sender_id = htons((u16)lobby_game.id);
        send(networkSocket, &h, sizeof(h), 0);
    }

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
