/**
 * @file main.c
 * @brief Client Lobby avec Réseau (Sockets TCP)
 */

#include "core/game.h"
#include "ui/app.h"
#include "ui/game.h"
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

#include "protocol.h" 

// --- CORRECTION PATH ASSETS (Macro stringification) ---
// Transforme les tokens non cotés (ex: lobby/assets/) en chaîne de caractères "lobby/assets/"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifdef ASSET_PATH
    #define ASSETS_DIR STR(ASSET_PATH)
#else
    #define ASSETS_DIR "lobby/assets/"
#endif

// On redéfinit IMAGES_PATH pour être sûr qu'il est correct
#undef IMAGES_PATH
#define IMAGES_PATH ASSETS_DIR "images/"


// --- CONFIGURATION ---
#define SERVER_IP "10.153.19.92"
#define SERVER_PORT 8080

// --- VARIABLES GLOBALES RÉSEAU ---
int network_socket = -1;
Player_st otherPlayers[MAX_CLIENTS]; 

// --- FONCTIONS RÉSEAU ---

void init_network() {
    struct sockaddr_in serv_addr;

    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur création socket");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Conversion IP
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        return;
    }

    if (connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connexion échouée");
        network_socket = -1;
        return;
    }

    printf(">>> CONNECTÉ AU SERVEUR !\n");

    // Mode NON-BLOQUANT
    fcntl(network_socket, F_SETFL, O_NONBLOCK);

    // Envoi JOIN
    PacketHeader header;
    header.room_id = htons(0); 
    header.sender_id = htons(0); // ID inconnu pour l'instant
    header.action = LOBBY_JOIN;
    header.length = htons(0);
    send(network_socket, &header, sizeof(header), 0);
}

void send_my_position(Player_st *p) {
    if (network_socket == -1) return;

    PacketHeader header;
    header.room_id = htons(0);
    // Note: Le serveur remplira le sender_id correct lors du broadcast
    header.sender_id = htons(0); 
    header.action = LOBBY_MOVE;
    header.length = htons(sizeof(Player_st));

    uint8_t buffer[sizeof(PacketHeader) + sizeof(Player_st)];
    
    memcpy(buffer, &header, sizeof(PacketHeader));
    memcpy(buffer + sizeof(PacketHeader), p, sizeof(Player_st));

    send(network_socket, buffer, sizeof(buffer), 0);
}

void receive_network_data() {
    if (network_socket == -1) return;

    uint8_t buffer[1024];
    ssize_t valread = recv(network_socket, buffer, sizeof(PacketHeader), MSG_PEEK);

    if (valread >= (ssize_t)sizeof(PacketHeader)) {
        // On consomme le header
        recv(network_socket, buffer, sizeof(PacketHeader), 0);
        
        PacketHeader *header = (PacketHeader *)buffer;
        uint16_t senderId = ntohs(header->sender_id);
        uint16_t dataLen = ntohs(header->length);
        uint8_t action = header->action;

        // Sécurité
        if (senderId >= MAX_CLIENTS) return; 

        if (dataLen > 0) {
            uint8_t payload[1024];
            int totalRead = 0;
            
            // Boucle de lecture sécurisée
            while(totalRead < dataLen) {
                ssize_t bytes = recv(network_socket, payload + totalRead, dataLen - totalRead, 0);
                if (bytes > 0) {
                    totalRead += bytes;
                } else if (bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    continue; 
                } else {
                    return; 
                }
            }

            if (action == LOBBY_MOVE && dataLen == sizeof(Player_st)) {
                Player_st *ghost = (Player_st *)payload;
                
                // 1. Copier les données reçues
                // ATTENTION: ghost->texture contient un pointeur invalide (adresse mémoire de l'autre PC)
                otherPlayers[senderId] = *ghost; 
                
                // 2. Assigner une texture LOCALE valide (pointeur)
                // On utilise la texture 1 (Troll) pour les autres joueurs
                otherPlayers[senderId].texture = &playerTextures[1]; 
            }
        }
    }
}

// --- GLOBALES JEU ---
static Player_st player = {
    .position = {0, 0}, .radius = 20, .velocity = {0, 0}, .onGround = false
};
static Camera2D cam = { .zoom = 1.0f };
static Rectangle gameZoneHitbox = { .x = 600, .y = -150, .width = 75, .height = 75 };


// --- MAIN LOOP ---

static void lobby_gameLoop(float dt) {
    static Vector2 lastSentPos = {0};

    // 1. Mouvement
    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    // 2. Réseau : Envoi si mouvement
    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y) {
        send_my_position(&player);
        lastSentPos = player.position;
    }

    // 3. Réseau : Réception
    receive_network_data();

    // 4. UI
    toggleSkinMenu();
    if (isTextureMenuOpen) choosePlayerTexture(&player);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(cam);
        
        drawPlatforms(platforms, platformCount);
        DrawCircle(0, 0, 10, RED);

        // DESSINER MON JOUEUR
        drawPlayer(&player);

        // DESSINER LES AUTRES JOUEURS
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // Vérifie si le joueur existe et a une texture valide
            if (otherPlayers[i].texture != NULL && otherPlayers[i].texture->id != 0) {
                
                // Note: On déréférence le pointeur (*...) pour passer la Structure Texture2D
                DrawTexturePro(
                    *otherPlayers[i].texture, 
                    (Rectangle){0, 0, (float)otherPlayers[i].texture->width, (float)otherPlayers[i].texture->height},
                    (Rectangle){otherPlayers[i].position.x, otherPlayers[i].position.y, 40, 40},
                    (Vector2){20, 20}, 
                    otherPlayers[i].angle, 
                    WHITE
                );
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

    // Chargement assets
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    
    // Chargement des textures joueurs
    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;
    
    // Assignation de l'ADRESSE de la texture (car .texture est un pointeur)
    player.texture = &playerTextures[0]; 
    
    cam.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};

    init_network();

    while (!WindowShouldClose()) {
        lobby_gameLoop(GetFrameTime());
    }

    if (network_socket != -1) close(network_socket);
    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"