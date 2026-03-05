/**
 * @file connection_screen.c
 * @brief Gestion de la scène "Pré-Lobby" avec découverte de salons.
 */

#include "raylib.h"
#include "ui/input_button.h"
#include "rudp_core.h"
#include <stdio.h>
#include <string.h>

#define MAX_ROOMS_DISPLAY 5

typedef struct {
    char ip[16];
    char name[32];
    IaC_button button;
    bool active;
} RoomEntry;

// État local
static IaC_button ipInput;
static IaC_button connectButton;
static IaC_button refreshButton;
static char ipBuffer[IP_MAX_LENGTH + 1] = {0};
static int letterCount = 0;

static RoomEntry discoveredRooms[MAX_ROOMS_DISPLAY];
static int roomsCount = 0;

void InitConnectionScreen(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    ipInput = InitIaCElement((float)(screenWidth/2 - 150), (float)(screenHeight/2 - 200), 300, 40, "IP (ex: 127.0.0.1)", LIGHTGRAY);
    connectButton = InitIaCElement((float)(screenWidth/2 - 100), (float)(screenHeight/2 - 125), 200, 40, "Se Connecter", DARKGRAY);
    
    // Bouton pour lancer le broadcast de découverte
    refreshButton = InitIaCElement((float)(screenWidth - 180), (float)(screenHeight/2 - 40), 160, 30, "Rafraichir", SKYBLUE);

    for (int i = 0; i < MAX_ROOMS_DISPLAY; i++) discoveredRooms[i].active = false;
    roomsCount = 0;
}

/**
 * @brief Simule l'ajout d'une room reçue par le réseau.
 * En production, cette fonction est appelée par le récepteur UDP de main.c.
 */
void AddDiscoveredRoom(const char* ip, const char* name) {
    if (roomsCount >= MAX_ROOMS_DISPLAY) return;
    
    // 1. Déclaration de i à l'intérieur du for pour la recherche de doublons
    for (int i = 0; i < roomsCount; i++) {
        if (strcmp(discoveredRooms[i].ip, ip) == 0) return;
    }

    // 2. Utilisation de roomsCount (l'index de la nouvelle entrée)
    strncpy(discoveredRooms[roomsCount].ip, ip, 15);
    strncpy(discoveredRooms[roomsCount].name, name, 31);
    
    float startY = (float)(GetScreenHeight() / 2 + 20);
    
    // 3. Cast en (char*) pour calmer le compilateur sur le "const"
    discoveredRooms[roomsCount].button = InitIaCElement(
        30, 
        startY + (roomsCount * 50), 
        (float)GetScreenWidth() - 60, 
        40, 
        (char*)name, 
        WHITE
    );
    
    discoveredRooms[roomsCount].active = true;
    roomsCount++;
}

// On déclare que cette fonction est définie dans un autre fichier (main.c)
extern void discover_servers(void);

bool UpdateConnectionScreen(void) {
    UpdateIPInput(&ipInput, ipBuffer, &letterCount);

    // Logique du bouton Rafraîchir
    if (UpdateConnectButton(&refreshButton, true)) {
        // --- ACTION RÉELLE ---
        // On vide la liste actuelle pour ne pas accumuler des vieux serveurs
        // (Optionnel : roomsCount = 0;)
        
        // On lance le SONAR (Broadcast UDP)
        discover_servers(); 
        
        // --- SUPPRESSION DE LA SIMULATION ---
        // Plus besoin de AddDiscoveredRoom("127.0.0.1", ...) ici !
    }

    // 3. Gestion du clic sur une room détectée par le réseau
    // C'est receive_network_data() dans main.c qui remplit ce tableau asynchronement
    for (int i = 0; i < roomsCount; i++) {
        if (UpdateConnectButton(&discoveredRooms[i].button, true)) {
            // Si on clique sur une room, on injecte son IP dans le champ de saisie
            strncpy(ipBuffer, discoveredRooms[i].ip, IP_MAX_LENGTH);
            letterCount = (int)strlen(ipBuffer);
            ipInput.isIPValid = true;
        }
    }

    // 4. Retourne true si on clique sur "Se Connecter"
    return UpdateConnectButton(&connectButton, ipInput.isIPValid);
}

void DrawConnectionScreen(void) {
    ClearBackground(RAYWHITE);
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("CONNEXION AU SERVEUR", sw/2 - MeasureText("CONNEXION AU SERVEUR", 30)/2, sh/2 - 250, 30, DARKGRAY);

    DrawIaCElement(ipInput, ipBuffer);
    DrawIaCElement(connectButton, "");
    DrawIaCElement(refreshButton, "");

    if (!ipInput.isIPValid && ipInput.state == STATE_ACTIVE) {
        DrawText("Format attendu : XXX.XXX.XXX.XXX", sw/2 - 100, sh/2 - 150, 15, RED);
    }

    // Zone de liste
    DrawText("Liste des rooms :", 20, sh/2 - 30, 25, DARKBLUE);
    DrawRectangle(20, (float)(sh/2), sw - 40, 275, LIGHTGRAY);
    DrawRectangleLines(20, (float)(sh/2), sw - 40, 275, GRAY);

    // Rendu des items cliquables
    for (int i = 0; i < roomsCount; i++) {
        DrawIaCElement(discoveredRooms[i].button, discoveredRooms[i].name);
        // On affiche l'IP à droite du bouton pour info
        DrawText(discoveredRooms[i].ip, sw - 180, (int)discoveredRooms[i].button.rect.y + 10, 18, GRAY);
    }
    
    if (roomsCount == 0) {
        DrawText("Aucun serveur détecté. Cliquez sur Rafraichir.", sw/2 - 150, sh/2 + 100, 15, DARKGRAY);
    }
}

const char* GetEnteredIP(void) { return ipBuffer; }