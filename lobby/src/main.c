/**
 * @file main.c
 * @brief Point d'entrée du client Lobby avec gestion réseau (Sockets TCP).
 * @author Fshimi Hawlk
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-02-15
 * Ce fichier gère l'affichage Raylib, la boucle de jeu locale, la
 * synchronisation des positions des joueurs via le serveur et les transitions de scène.
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

#include "protocol.h"

// --- CORRECTION PATH ASSETS (Macro stringification) ---
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

// --- GESTION DES SCÈNES (State Machine) ---
typedef enum {
    STATE_CONNECTION, // Scène de saisie de l'IP
    STATE_LOBBY       // Scène principale (Jeu + Réseau)
} GameState;

static GameState currentState = STATE_CONNECTION; // État de départ

// --- VARIABLES GLOBALES RÉSEAU ---
int network_socket = -1;
Player_st otherPlayers[MAX_CLIENTS];

// --- VARIABLES GLOBALES JEU ---
static Player_st player = {
    .position = {0, 0}, .radius = 20, .velocity = {0, 0}, .onGround = false
};
static Camera2D cam = { .zoom = 1.0f };
static Rectangle gameZoneHitbox = { .x = 600, .y = -150, .width = 75, .height = 75 };


// --- FONCTIONS RÉSEAU ---

/**
 * @brief Initialise la connexion TCP vers le serveur.
 * @param target_ip L'adresse IPv4 cible (Ex: "127.0.0.1")
 */
void init_network(const char* target_ip) {
    struct sockaddr_in serv_addr;

    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erreur création socket");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Conversion IP dynamique
    if (inet_pton(AF_INET, target_ip, &serv_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        return;
    }

    if (connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connexion échouée");
        network_socket = -1;
        return;
    }

    printf(">>> CONNECTÉ AU SERVEUR (%s) !\n", target_ip);

    // Mode NON-BLOQUANT pour éviter le gel de la fenêtre de rendu
    fcntl(network_socket, F_SETFL, O_NONBLOCK);

    // Envoi JOIN initial
    PacketHeader header;
    header.room_id = htons(0);
    header.sender_id = htons(0); 
    header.action = LOBBY_JOIN;
    header.length = htons(0);
    send(network_socket, &header, sizeof(header), 0);
}

/**
 * @brief Envoie la position locale au serveur.
 * @param p Pointeur vers la structure du joueur local.
 */
void send_my_position(Player_st *p) {
    if (network_socket == -1) return;

    PacketHeader header;
    header.room_id = htons(0);
    header.sender_id = htons(0);
    header.action = LOBBY_MOVE;
    header.length = htons(sizeof(Player_st));

    uint8_t buffer[sizeof(PacketHeader) + sizeof(Player_st)];

    memcpy(buffer, &header, sizeof(PacketHeader));
    memcpy(buffer + sizeof(PacketHeader), p, sizeof(Player_st));

    send(network_socket, buffer, sizeof(buffer), 0);
}

/**
 * @brief Vérifie et traite les données reçues sur la socket.
 * Protège contre le Spinlock et les paquets corrompus.
 */
void receive_network_data(void) {
    if (network_socket == -1) return;

    uint8_t buffer[1024];
    // Observation de l'en-tête (MSG_PEEK) pour ne pas altérer le flux prématurément
    ssize_t valread = recv(network_socket, buffer, sizeof(PacketHeader), MSG_PEEK);

    if (valread == (ssize_t)sizeof(PacketHeader)) {
        PacketHeader *header = (PacketHeader *)buffer;
        uint16_t dataLen = ntohs(header->length);
        
        // Sécurité anti-débordement
        if (dataLen > 512) {
            recv(network_socket, buffer, sizeof(PacketHeader), 0);
            return;
        }

        // On consomme l'en-tête validé
        recv(network_socket, buffer, sizeof(PacketHeader), 0);
        
        uint16_t senderId = ntohs(header->sender_id);
        uint8_t action = header->action;

        if (senderId >= MAX_CLIENTS) return;

        if (dataLen > 0) {
            uint8_t payload[1024];
            int totalRead = 0;

            // Boucle de lecture asynchrone sécurisée
            while(totalRead < dataLen) {
                ssize_t bytes = recv(network_socket, payload + totalRead, dataLen - totalRead, 0);
                
                if (bytes > 0) {
                    totalRead += bytes;
                } else if (bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    // Rupture du blocage pour permettre à Raylib de dessiner la frame
                    break;
                } else {
                    return;
                }
            }

            // Traitement conditionné à la réception intégrale du payload
            if (totalRead == dataLen) {
                if (action == LOBBY_MOVE && dataLen == sizeof(Player_st)) {
                    Player_st *ghost = (Player_st *)payload;
                    otherPlayers[senderId] = *ghost;
                    otherPlayers[senderId].texture = &playerTextures[1];
                }
            }
        }
    }
}

/**
 * @brief Boucle de mise à jour et de rendu (Tick client).
 * @param dt Delta time (temps écoulé depuis la dernière frame).
 */
static void lobby_gameLoop(float dt) {
    static Vector2 lastSentPos = {0};

    // 1. Mouvement et Physique
    updatePlayer(&player, platforms, platformCount, dt);
    cam.target = player.position;

    // 2. Réseau : Envoi si modification de la géométrie locale
    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y) {
        send_my_position(&player);
        lastSentPos = player.position;
    }

    // 3. Réseau : Réception des événements asynchrones
    receive_network_data();

    // 4. UI locale
    toggleSkinMenu();
    if (isTextureMenuOpen) choosePlayerTexture(&player);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode2D(cam);

        drawPlatforms(platforms, platformCount);
        DrawCircle(0, 0, 10, RED);

        // Rendu du joueur local
        drawPlayer(&player);

        // Rendu des homologues distants
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (otherPlayers[i].texture != NULL && otherPlayers[i].texture->id != 0) {
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

/**
 * @brief Entrée principale : initialise la fenêtre et le contrôleur de scènes.
 */
int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Multi-Mini-Games: CLIENT");

    // Chargement assets locaux
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");

    playerTextures[0] = LoadTexture(IMAGES_PATH "earth.png"); playerTextureCount++;
    playerTextures[1] = LoadTexture(IMAGES_PATH "trollFace.png"); playerTextureCount++;

    player.texture = &playerTextures[0];
    cam.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};

    // Allocation des ressources de la scène initiale
    InitConnectionScreen();

    // Boucle Principale - Automate à États Finis
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