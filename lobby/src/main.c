/*
    @author Fshimi Hawlk
    @author LeandreB8
    @author i-Charlys
    @date 2026-02-08
    @date 2026-04-02
    @brief Program entry point for the lobby client – lobby main loop, game scene manager, networking and module dispatching.
*/

#include "ui/menus.h"
#include "ui/connectionScreen.h"
#include "ui/roomSelector.h"
#include "ui/waitingRoom.h"

#include "setups/app.h"

#include "utils/globals.h"

s32 networkSocket = -1;
RUDPConnection_St serverConnection; 

static MiniGameId_Et currentMiniGameID = MINI_GAME_ID_LOBBY;
static GameClientInterface_St* currentMiniGame = NULL;

extern GameClientInterface_St bingo_clientInterface;
extern GameClientInterface_St bowling_clientInterface;
extern GameClientInterface_St chess_clientInterface;
// extern GameClientInterface_St discReversal_clientInterface;
// extern GameClientInterface_St dropFour_clientInterface;
extern GameClientInterface_St editor_clientInterface;
extern GameClientInterface_St kingForFour_clientInterface;
extern GameClientInterface_St kingForFour_soloClientInterface;
extern GameClientInterface_St lobby_clientInterface;
extern GameClientInterface_St miniGolf_clientInterface;
extern GameClientInterface_St polyBlast_clientInterface;
extern GameClientInterface_St snake_clientInterface;
extern GameClientInterface_St soloCards_clientInterface;
extern GameClientInterface_St suika_clientInterface;
extern GameClientInterface_St tetrominoFall_clientInterface;
extern GameClientInterface_St twistCube_clientInterface;

// Pointers to the mini-game client interfaces
static GameClientInterface_St* miniGameInterfaces[__miniGameIdCount] = {
    [MINI_GAME_ID_BINGO]          = &bingo_clientInterface,
    [MINI_GAME_ID_BOWLING]        = &bowling_clientInterface,
    [MINI_GAME_ID_CHESS]          = &chess_clientInterface,
    [MINI_GAME_ID_DISC_REVERSAL]  = NULL, // TODO
    [MINI_GAME_ID_DROP_FOUR]      = NULL, // TODO
    [MINI_GAME_ID_EDITOR]         = &editor_clientInterface,
    [MINI_GAME_ID_KING_FOR_FOUR]  = &kingForFour_clientInterface,
    [MINI_GAME_ID_LOBBY]          = &lobby_clientInterface,
    [MINI_GAME_ID_MINI_GOLF]      = &miniGolf_clientInterface,
    [MINI_GAME_ID_POLY_BLAST]     = &polyBlast_clientInterface,
    [MINI_GAME_ID_SNAKE]          = &snake_clientInterface,
    [MINI_GAME_ID_SOLO_CARDS]     = &soloCards_clientInterface,
    [MINI_GAME_ID_SUIKA]          = &suika_clientInterface,
    [MINI_GAME_ID_TETROMINO_FALL] = &tetrominoFall_clientInterface,
    [MINI_GAME_ID_TWIST_CUBE]     = &twistCube_clientInterface,
};

// Solo mode interfaces (for games that support local play with bots)
GameClientInterface_St* soloInterfaces[__miniGameIdCount] = {
    [MINI_GAME_ID_CHESS]          = &chess_clientInterface,  // Chess has built-in AI
    [MINI_GAME_ID_KING_FOR_FOUR]  = &kingForFour_soloClientInterface,
};

static bool server_spawned = false;
static s32 lastGameZoneIndex = -1;

#ifndef _WIN32
static pid_t server_pid = -1;
#endif

void kill_server(void) {
    if (server_spawned) {
        log_info("Arrêt du serveur local...");
#ifdef _WIN32
        int ret = system("taskkill /F /IM server.exe > nul 2>&1");
        (void)ret;
#else
        if (server_pid > 0) {
            kill(server_pid, SIGTERM);
            server_pid = -1;
        } else {
            int ret = system("killall server 2>/dev/null");
            (void)ret;
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

    RUDPHeader_St h_disc = { .action = ACTION_CODE_DISCOVERY_QUERY, .sequence = 0, .ack = 0, .ackBitfield = 0, .senderId = 0 };
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
            if (header.action == ACTION_CODE_DISCOVERY_INFO && ntohs(header.senderId) == 999) {
                char* info = (char*)buffer + sizeof(RUDPHeader_St);
                log_info("[DISCOVERY] Found Server at %s: %s", inet_ntoa(fromAddr.sin_addr), info);
                lobby_addDiscoveredServer(inet_ntoa(fromAddr.sin_addr), info);
                continue;
            }

            if (!rudpProcessIncoming(&serverConnection, &header)) continue;
        
            u8* payload = buffer + sizeof(RUDPHeader_St);
            u16 payloadLen = (u16) (bytesRead - sizeof(RUDPHeader_St));
            u16 senderId = ntohs(header.senderId);

            if (header.action == ACTION_CODE_JOIN_ERROR) {
                log_error("[NET] Join rejected by server: %s", (char*)payload);
                lobby_setConnectionError((char*)payload);
                lobby_game.currentState = GAME_STATE_CONNECTION;
                // Reset network state if needed
                rudpInitConnection(&serverConnection);

            } else if (header.action == ACTION_CODE_LOBBY_ROOM_INFO) {
                lobby_handleRoomList(payload, payloadLen / sizeof(RoomInfo_St));

            } else if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                if (payloadLen < 2) return;
                u8 nextGame = payload[0];
                u8 roomId = payload[1];
                log_info("[NET] Server confirmed switch to Game %d, Room %d", nextGame, roomId);

                lobby_closeRoomSelector();

                if (nextGame < __miniGameIdCount) {
                    if (currentMiniGame && currentMiniGame->destroy) {
                        currentMiniGame->destroy();
                    }
                    currentMiniGameID = (MiniGameId_Et) nextGame;
                    currentMiniGame = miniGameInterfaces[currentMiniGameID];
                    if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();
                    lobby_game.currentState = (nextGame == MINI_GAME_ID_LOBBY) ? GAME_STATE_GAMEPLAY : GAME_STATE_INGAME;
                    if (nextGame == MINI_GAME_ID_LOBBY) lobby_initWaitingRoom();
                }

            } else if (header.action == ACTION_CODE_GAME_DATA) {
                if (payloadLen < sizeof(GameTLVHeader_St)) return;

                GameTLVHeader_St tlv;
                memcpy(&tlv, payload, sizeof(tlv));
                u16 tlv_data_len = ntohs(tlv.length);
                
                if (tlv_data_len <= payloadLen - sizeof(GameTLVHeader_St)) {
                    if (tlv.gameId < __miniGameIdCount && miniGameInterfaces[tlv.gameId]) {
                        miniGameInterfaces[tlv.gameId]->onData(senderId, tlv.action, payload + sizeof(tlv), tlv_data_len);
                    }
                }

            } else if (currentMiniGame && currentMiniGame->onData) {
                if (header.action < firstAvailableActionCode || header.action == ACTION_CODE_JOIN_ACK) {
                    currentMiniGame->onData(senderId, header.action, payload, payloadLen);
                }
            }
        }
    }
}

void spawn_server(void) {
    if (!server_spawned) {
        log_info("Lancement du serveur local...");
#ifdef _WIN32
        (void)system("start /B server.exe");
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

void switchMinigame(u8 gameId) {
    if (gameId >= __miniGameIdCount) return;

    if (miniGameInterfaces[gameId]) {
        // Notify server that we are leaving the current minigame room (if any)
        if (currentMiniGameID != MINI_GAME_ID_LOBBY && currentMiniGameID != MINI_GAME_ID_EDITOR && gameId == MINI_GAME_ID_LOBBY) {
            GameTLVHeader_St tlv = { .gameId = currentMiniGameID, .action = ACTION_CODE_QUIT_GAME, .length = 0 };
            RUDPHeader_St h;
            rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
            h.senderId = htons((u16) lobby_game.clientId);
            u8 buf[64];
            memcpy(buf, &h, sizeof(h));
            memcpy(buf + sizeof(h), &tlv, sizeof(tlv));
            send(networkSocket, buf, sizeof(h) + sizeof(tlv), 0);
        }

        if (currentMiniGame && currentMiniGame->destroy) {
            currentMiniGame->destroy();
        }

        currentMiniGameID = (MiniGameId_Et) gameId;
        
        // Use solo interface if available and not connected to a server
        if (networkSocket < 0 && soloInterfaces[currentMiniGameID] != NULL) {
            currentMiniGame = soloInterfaces[currentMiniGameID];
        } else {
            currentMiniGame = miniGameInterfaces[currentMiniGameID];
        }

        if (currentMiniGame && currentMiniGame->init) currentMiniGame->init();

        // Ensure UI state is cleared when switching
        lobby_game.currentState = GAME_STATE_GAMEPLAY;
        lobby_closeRoomSelector();

        if (gameId == MINI_GAME_ID_LOBBY) {
            lobby_game.currentState = GAME_STATE_GAMEPLAY;
            lobby_game.editorMode = false;
            lobby_initWaitingRoom();
            if (lastGameZoneIndex >= 0 && lastGameZoneIndex < __miniGameIdCount) {
                Rectangle zone = gameZones[lastGameZoneIndex].hitbox;
                lobby_game.player.position.x = zone.x + zone.width / 2.0f;
                lobby_game.player.position.y = zone.y + zone.height + lobby_game.player.radius + 10.0f;
                lobby_game.player.velocity = (Vector2){0, 0};
            }
        } else {
            lobby_game.currentState = GAME_STATE_INGAME;
            if (gameId == MINI_GAME_ID_EDITOR) lobby_game.editorMode = true;
        }

        log_info("Switched to mini-game ID: %d", gameId);
    } else {
        log_warn("Not Implemented Yet");
    }
}

void setCurrentMiniGame(GameClientInterface_St* iface) {
    currentMiniGame = iface;
    if (iface && iface->init) iface->init();
}

int main(void) {
    g_progress = LoadProgress();
    
    if (lobby_initApp() != OK) return 1;

    lobby_initMenus();

    miniGameInterfaces[MINI_GAME_ID_LOBBY]->init();
    currentMiniGame = miniGameInterfaces[MINI_GAME_ID_LOBBY];
    
    lobby_initConnectionScreen();
    // Use the default pseudo from the connection screen as the initial player name
    strncpy(lobby_game.player.name, lobby_getEnteredPseudo(), 31);

    lobby_initRoomSelector();
    lobby_initWaitingRoom();

    while (!WindowShouldClose() && !g_shouldExit) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f;
        
        receiveNetworkData();

        lobby_updateMenu();
        bool selectorActive = lobby_updateRoomSelector();
        // L'overlay "salle d'attente" du lobby n'est actif qu'en lobby :
        // chaque module de jeu gère sa propre phase pré-partie.
        if (currentMiniGameID == MINI_GAME_ID_LOBBY) lobby_updateWaitingRoom();

        // Update mini-game BEFORE BeginDrawing: solo games call BeginDrawing/EndDrawing
        // themselves inside their update — calling them from within the lobby's BeginDrawing
        // would produce a nested BeginDrawing. The lobby module itself is safe to update inside.
        if (currentMiniGame && currentMiniGameID != MINI_GAME_ID_LOBBY) {
            currentMiniGame->update(dt);
        }

        BeginDrawing(); {
            // Only clear background if we are in the lobby itself.
            // Mini-games manage their own background clearing.
            if (currentMiniGameID == MINI_GAME_ID_LOBBY || !currentMiniGame) {
                ClearBackground((Color){25, 84, 157, 255}); // sky top colour
            }

            if (lobby_currentMenu != MENU_NONE) {
                lobby_drawMenu();
            } else if (lobby_game.currentState == GAME_STATE_CONNECTION) {
                static f32 disc_timer = 0;
                disc_timer += dt;
                if (disc_timer > 2.0f) { discoverServers(); disc_timer = 0; }
                
                if (lobby_updateConnectionScreen()) {
                    const char* pseudo = lobby_getEnteredPseudo();
                    initNetwork(lobby_getEnteredIP(), pseudo);
                    strncpy(lobby_game.player.name, pseudo, 31);
                    lobby_game.currentState = GAME_STATE_ROOM_LIST;
                    lobby_openRoomSelector(-1); // Open global room list
                }
                lobby_drawConnectionScreen();
            } else if (lobby_game.currentState == GAME_STATE_ROOM_LIST) {
                lobby_drawRoomSelector();
            } else {
                // Gameplay loop
                static MiniGameId_Et lastTriggerID = MINI_GAME_ID_LOBBY;

                if (currentMiniGameID == MINI_GAME_ID_LOBBY && !lobby_game.editorMode && !selectorActive) {
                    bool isConnected = networkSocket >= 0;
                    for (int i = 0; i < __miniGameIdCount; i++) {
                        gameZones[i].isRestricted = false;
                        if (i == MINI_GAME_ID_EDITOR) {
                            if (isConnected && !server_spawned) {
                                gameZones[i].isRestricted = true;
                            }
                        } else {
                            bool isImplemented = miniGameInterfaces[i] != NULL;
                            bool hasAI = (i == MINI_GAME_ID_CHESS || i == MINI_GAME_ID_KING_FOR_FOUR);
                            if (!isImplemented && !hasAI && !isConnected) {
                                gameZones[i].isRestricted = true;
                            }
                        }
                    }

                    MiniGameId_Et triggerID = MINI_GAME_ID_LOBBY;
                    for (int i = 0; i < __miniGameIdCount; i++) {
                        if (gameZones[i].hitbox.width <= 0 || gameZones[i].hitbox.height <= 0) continue;
                        if (CheckCollisionCircleRec(lobby_game.player.position, lobby_game.player.radius, gameZones[i].hitbox)) {
                            triggerID = i;
                            break;
                        }
                    }

                    if (triggerID != MINI_GAME_ID_LOBBY && !gameZones[triggerID].isRestricted) {
                        if (lastTriggerID != triggerID) {
                            // log_info("[ZONE] Entered Zone for Game ID %d", triggerID);
                            lastTriggerID = triggerID;
                        }

                        if (IsKeyPressed(KEY_E)) {
                            // Logique solo/multi basée sur l'interface réseau du jeu
                            // Si le jeu a une interface réseau ET qu'on est connecté → multi
                            // Sinon → solo (utilise l'interface solo si disponible)
                            bool hasMulti = miniGameInterfaces[triggerID] != NULL;
                            bool isConnected = lobby_game.clientId != -1;
                            
                            if (hasMulti && isConnected && triggerID != MINI_GAME_ID_EDITOR) {
                                lobby_openRoomSelector(triggerID);  // multi
                            } else {
                                lastGameZoneIndex = triggerID;
                                switchMinigame(triggerID);          // solo
                            }
                        }
                    } else {
                        if (lastTriggerID != MINI_GAME_ID_LOBBY) {
                            lobby_closeRoomSelector();
                            lastTriggerID = MINI_GAME_ID_LOBBY;
                        }
                    }
                }

                if (currentMiniGame) {
                    // Lobby module update is safe inside BeginDrawing (no nested BeginDrawing).
                    // Other games already had their update() called above.
                    if (currentMiniGameID == MINI_GAME_ID_LOBBY) {
                        currentMiniGame->update(dt);
                    }
                    currentMiniGame->draw();
                }
                
                lobby_drawRoomSelector();
                if (currentMiniGameID == MINI_GAME_ID_LOBBY) lobby_drawWaitingRoom();
            }

            DrawFPS(10, 10);
        } EndDrawing();
    }

    SaveProgress(&g_progress);

    if (networkSocket != -1) {
        RUDPHeader_St h;
        rudpGenerateHeader(&serverConnection, ACTION_CODE_QUIT_GAME, &h);
        h.senderId = htons((u16)lobby_game.clientId);
        send(networkSocket, &h, sizeof(h), 0);
    }

    kill_server();
    lobby_freeApp();

    return 0;
}