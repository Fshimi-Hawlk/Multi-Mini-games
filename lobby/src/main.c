/**
 * @file main.c
 * @brief Point d'entrée du client Lobby avec gestion réseau (Datagrammes UDP + RUDP).
 * @author i-Charlys (CAILLON Charles)

 * @date 2026-03-05
 */

#include "core/game.h"
#include "ui/app.h"
#include "ui/game.h"
#include "ui/connection_screen.h"
#include "utils/globals.h"

// --- INCLUDES RÉSEAU ---
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

typedef enum {
    STATE_CONNECTION,
    STATE_LOBBY
} GameState;

static GameState currentState = STATE_CONNECTION;

// --- VARIABLES GLOBALES RÉSEAU ---
int network_socket = -1;
RUDP_Connection server_conn; 
Player_st otherPlayers[MAX_CLIENTS];

// --- VARIABLES GLOBALES JEU ---
static Player_st player = {
    .position = {0, 0}, .radius = 20, .velocity = {0, 0}, .onGround = false
};
static Camera2D cam = { .zoom = 1.0f };
static Rectangle gameZoneHitbox = { .x = 600, .y = -150, .width = 75, .height = 75 };


// --- FONCTIONS RÉSEAU ---

void init_network(const char* target_ip) {
    struct sockaddr_in serv_addr;

    // 1. Création de la socket
    if ((network_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Erreur création socket UDP");
        return;
    }

    // 2. Activation du Broadcast (Optionnel ici, mais utile pour la découverte plus tard)
    int broadcastPermission = 1;
    setsockopt(network_socket, SOL_SOCKET, SO_BROADCAST, &broadcastPermission, sizeof(broadcastPermission));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, target_ip, &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        return;
    }

    // 3. Liaison (Connect UDP ne fait pas de Handshake, il définit juste la cible par défaut)
    if (connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erreur liaison adresse UDP");
        network_socket = -1;
        return;
    }

    // 4. Initialisation RUDP & Non-bloquant
    RUDP_InitConnection(&server_conn);
    fcntl(network_socket, F_SETFL, O_NONBLOCK);

    printf(">>> ROUTAGE UDP PRÊT VERS LE SERVEUR (%s) !\n", target_ip);

    // 5. Envoi du paquet de Join
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


void receive_network_data(void) {
    if (network_socket == -1) return;

    uint8_t buffer[2048];
    while (1) {
        ssize_t valread = recv(network_socket, buffer, sizeof(buffer), 0);
        
        if (valread < 0) {
            // EAGAIN est normal en non-bloquant
            break; 
        }

        // LOG DE RÉCEPTION BRUTE
        printf("[RÉSEAU] Paquet reçu ! Taille: %ld octets\n", valread);

        if (valread >= (ssize_t)sizeof(RUDP_Header)) {
            RUDP_Header *header = (RUDP_Header *)buffer;
            
            if (RUDP_ProcessIncoming(&server_conn, header)) {
                uint16_t senderId = ntohs(header->sender_id);
                uint8_t action = header->action;
                uint16_t dataLen = valread - sizeof(RUDP_Header);

                printf("[RUDP] Paquet validé - Action: %d, Expéditeur: %d\n", action, senderId);

                if (senderId < MAX_CLIENTS && action == LOBBY_MOVE) {
                    memcpy(&otherPlayers[senderId], buffer + sizeof(RUDP_Header), sizeof(Player_st));
                    otherPlayers[senderId].texture = &playerTextures[1];
                }
            } else {
                printf("[RUDP] Paquet rejeté (Séquence périmée ou doublon)\n");
            }
        }
    }
}

static void lobby_gameLoop(float dt) {
    static Vector2 lastSentPos = {0};
    static float timeSinceLastPacket = 0.0f;

    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    timeSinceLastPacket += dt;
    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y || timeSinceLastPacket > 1.0f) {
        send_my_position(&player);
        lastSentPos = player.position;
        timeSinceLastPacket = 0.0f;
    }

    receive_network_data();

    toggleSkinMenu();
    if (isTextureMenuOpen) choosePlayerTexture(&player);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(cam);

        drawPlatforms(platforms, platformCount);
        DrawCircle(0, 0, 10, RED);

        drawPlayer(&player);

        // Moteur de rendu des autres joueurs avec Fallback de sécurité
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // Un joueur n'est dessiné que si on a reçu sa texture une fois
            if (otherPlayers[i].texture != NULL) {
                if (otherPlayers[i].texture->id != 0) {
                    DrawTexturePro(
                        *otherPlayers[i].texture,
                        (Rectangle){0, 0, (float)otherPlayers[i].texture->width, (float)otherPlayers[i].texture->height},
                        (Rectangle){otherPlayers[i].position.x, otherPlayers[i].position.y, 40, 40},
                        (Vector2){20, 20},
                        otherPlayers[i].angle,
                        WHITE
                    );
                } else {
                    // FALLBACK ABSOLU : Si l'image n'existe pas, dessine un grand cercle Rouge vif
                    DrawCircle((int)otherPlayers[i].position.x, (int)otherPlayers[i].position.y, 25, RED);
                }
            }
        }

        DrawRectangleRec(gameZoneHitbox, Fade(PURPLE, 0.5f));

    EndMode2D();
    
    DrawText("Lobby Multijoueur", 10, 10, 20, PURPLE);
    drawSkinButton();
    if (isTextureMenuOpen) drawMenuTextures();
    
    EndDrawing();
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Multi-Mini-Games: CLIENT");

    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;

    player.texture = &playerTextures[0];
    cam.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};

    // Initialisation globale pour éviter les artefacts visuels
    for (int i=0; i < MAX_CLIENTS; i++) {
        otherPlayers[i].texture = NULL;
    }

    InitConnectionScreen();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        switch (currentState) {
            case STATE_CONNECTION:
                if (UpdateConnectionScreen()) {
                    const char* targetIP = GetEnteredIP();
                    init_network(targetIP);
                    currentState = STATE_LOBBY;
                }
                BeginDrawing();
                DrawConnectionScreen();
                EndDrawing();
                break;

            case STATE_LOBBY:
                lobby_gameLoop(dt);
                break;
        }
    }

    if (network_socket != -1) close(network_socket);
    CloseWindow();
    return 0;
}