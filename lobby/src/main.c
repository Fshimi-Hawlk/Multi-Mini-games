/**
 * @file main.c
 * @brief Point d'entrée du client Lobby - Architecture RUDP & Service Discovery.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#include "core/game.h"
#include "ui/app.h"
#include "ui/game.h"
#include "ui/connection_screen.h"
#include "utils/globals.h"

// --- INCLUDES SYSTÈME & RÉSEAU ---
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>

#include "rudp_core.h" 

// --- MACROS ASSETS ---
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#ifdef ASSET_PATH
    #define ASSETS_DIR STR(ASSET_PATH)
#else
    #define ASSETS_DIR "lobby/assets/"
#endif
#undef IMAGES_PATH
#define IMAGES_PATH ASSETS_DIR "images/"

#define SERVER_PORT 8080

// --- ÉTATS DU JEU ---
typedef enum {
    STATE_CONNECTION,
    STATE_LOBBY
} GameState;

static GameState currentState = STATE_CONNECTION;

// --- GLOBALES RÉSEAU ---
int network_socket = -1;
RUDP_Connection server_conn; 
Player_st otherPlayers[MAX_CLIENTS];

// --- GLOBALES JEU ---
static Player_st player = { .position = {0, 0}, .radius = 20, .velocity = {0, 0}, .onGround = false };
static Camera2D cam = { .zoom = 1.0f };
static Rectangle gameZoneHitbox = { .x = 600, .y = -150, .width = 75, .height = 75 };

// --- LOGIQUE RÉSEAU ---

/**
 * @brief Prépare la socket dès le lancement pour permettre la découverte.
 */
void ensure_socket_exists() {
    if (network_socket != -1) return;

    if ((network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("[ERREUR] Création socket UDP");
        return;
    }

    // Autoriser l'envoi vers l'adresse de broadcast (255.255.255.255)
    int broadcastPermission = 1;
    setsockopt(network_socket, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission));

    // Mode non-bloquant pour l'UI Raylib
    fcntl(network_socket, F_SETFL, O_NONBLOCK);
    printf("[RÉSEAU] Socket UDP prête pour la découverte.\n");
}

/**
 * @brief Envoie une requête de détection sur tout le réseau local.
 */
void discover_servers(void) {
    ensure_socket_exists();
    
    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(SERVER_PORT);
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST; 

    RUDP_Header query;
    memset(&query, 0, sizeof(RUDP_Header));
    query.action = LOBBY_ROOM_QUERY; 

    sendto(network_socket, &query, sizeof(RUDP_Header), 0, 
           (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
    
    printf("[SONAR] Requête de découverte diffusée...\n");
}

/**
 * @brief Lie la socket à un serveur spécifique après sélection dans l'UI.
 */
void init_network(const char* target_ip) {
    ensure_socket_exists();
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, target_ip, &serv_addr.sin_addr) <= 0) {
        perror("[ERREUR] IP cible invalide");
        return;
    }

    // Le connect() UDP filtre les paquets pour ne recevoir QUE ceux de ce serveur
    if (connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[ERREUR] Échec du verrouillage UDP");
        return;
    }

    RUDP_InitConnection(&server_conn);
    printf(">>> RÉSEAU SYNCHRONISÉ AVEC %s <<<\n", target_ip);

    // Envoi du paquet d'entrée (JOIN)
    RUDP_Header header;
    RUDP_GenerateHeader(&server_conn, LOBBY_JOIN, &header);
    header.sender_id = htons(0);
    send(network_socket, &header, sizeof(RUDP_Header), 0);
}

void send_my_position(Player_st *p) {
    if (network_socket == -1) return;

    RUDP_Header header;
    RUDP_GenerateHeader(&server_conn, LOBBY_MOVE, &header);
    header.sender_id = htons(0); 

    uint8_t buffer[sizeof(RUDP_Header) + sizeof(Player_st)];
    memcpy(buffer, &header, sizeof(RUDP_Header));
    memcpy(buffer + sizeof(RUDP_Header), p, sizeof(Player_st));

    send(network_socket, buffer, sizeof(buffer), 0);
}

/**
 * @brief Aspirateur de datagrammes - Gère la découverte ET le jeu.
 */
void receive_network_data(void) {
    if (network_socket == -1) return;

    uint8_t buffer[2048];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    while (1) {
        ssize_t valread = recvfrom(network_socket, buffer, sizeof(buffer), 0, 
                                   (struct sockaddr*)&from_addr, &from_len);
        
        if (valread < 0) break; // Plus rien dans le tampon OS

        if (valread >= (ssize_t)sizeof(RUDP_Header)) {
            RUDP_Header *header = (RUDP_Header *)buffer;
            
            // 1. GESTION DE LA DÉCOUVERTE (Même sans session RUDP valide)
            if (header->action == LOBBY_ROOM_INFO) {
                char* real_ip = inet_ntoa(from_addr.sin_addr);
                char* server_name = (char*)(buffer + sizeof(RUDP_Header));
                AddDiscoveredRoom(real_ip, server_name);
                continue;
            }

            // 2. GESTION DU JEU (Filtre de fiabilité RUDP)
            if (RUDP_ProcessIncoming(&server_conn, header)) {
                uint16_t senderId = ntohs(header->sender_id);
                if (senderId < MAX_CLIENTS && header->action == LOBBY_MOVE) {
                    memcpy(&otherPlayers[senderId], buffer + sizeof(RUDP_Header), sizeof(Player_st));
                    otherPlayers[senderId].texture = &playerTextures[1]; // Trollface par défaut
                }
            }
        }
    }
}

// --- BOUCLES DE JEU ---

static void lobby_gameLoop(float dt) {
    static Vector2 lastSentPos = {0};
    static float timeSinceLastPacket = 0.0f;

    // Physique
    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    // Émission réseau périodique ou sur mouvement
    timeSinceLastPacket += dt;
    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y || timeSinceLastPacket > 1.0f) {
        send_my_position(&player);
        lastSentPos = player.position;
        timeSinceLastPacket = 0.0f;
    }

    receive_network_data();

    // Rendu
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(cam);
        drawPlatforms(platforms, platformCount);
        DrawCircle(0, 0, 10, RED);
        drawPlayer(&player);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (otherPlayers[i].texture != NULL) drawPlayer(&otherPlayers[i]);
        }
        DrawRectangleRec(gameZoneHitbox, Fade(PURPLE, 0.5f));
    EndMode2D();
    
    DrawText("Lobby Multijoueur", 10, 10, 20, PURPLE);
    EndDrawing();
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Multi-Mini-Games: RUDP CLIENT");

    // Chargement Assets
    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;
    player.texture = &playerTextures[0];
    
    cam.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    for (int i=0; i < MAX_CLIENTS; i++) otherPlayers[i].texture = NULL;

    InitConnectionScreen();
    ensure_socket_exists(); // Initialisation immédiate pour permettre le rafraîchissement
    discover_servers();
    
    while (!WindowShouldClose()) {
            float dt = GetFrameTime();
    
            switch (currentState) {
                case STATE_CONNECTION: {
                    // Gestion du rafraîchissement automatique (Toutes les 2 secondes)
                    static float discoveryTimer = 0.0f;
                    discoveryTimer += dt;
                    if (discoveryTimer >= 2.0f) {
                        discover_servers();
                        discoveryTimer = 0.0f;
                    }
    
                    // ÉCOUTE PERMANENTE
                    receive_network_data();
    
                    if (UpdateConnectionScreen()) {
                        init_network(GetEnteredIP());
                        currentState = STATE_LOBBY;
                    }
                    
                    BeginDrawing();
                    DrawConnectionScreen();
                    EndDrawing();
                    break;
                }
    
                case STATE_LOBBY:
                    lobby_gameLoop(dt);
                    break;
            }
        }

    if (network_socket != -1) close(network_socket);
    CloseWindow();
    return 0;
}