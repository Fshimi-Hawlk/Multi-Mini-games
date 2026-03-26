/**
 * @file connection_screen.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of the connection screen for server discovery and IP entry.
 */

#include <string.h>

#include "ui/input_button.h"

/** @brief Maximum number of discovered rooms to display. */
#define MAX_ROOMS_DISPLAY 5

/**
 * @brief Structure representing a discovered room entry in the UI.
 */
typedef struct {
    char ip[16];           ///< IP address of the room.
    char name[32];         ///< Name of the room.
    IaC_button button;     ///< UI button for the room entry.
    bool active;           ///< Whether this entry is active.
} RoomEntry;

/** @brief UI input for the IP address. */
static IaC_button ipInput;
/** @brief UI button to trigger connection. */
static IaC_button connectButton;
/** @brief UI button to refresh the server list. */
static IaC_button refreshButton;
/** @brief Buffer for the entered IP address. */
static char ipBuffer[IP_MAX_LENGTH + 1] = {0};
/** @brief Current character count in the IP buffer. */
static int letterCount = 0;

/** @brief Array of discovered rooms. */
static RoomEntry discoveredRooms[MAX_ROOMS_DISPLAY];
/** @brief Current number of discovered rooms. */
static int roomsCount = 0;

/**
 * @brief Allocates and positions UI elements for the connection screen.
 */
void initConnectionScreen(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    ipInput = initIaCElement(screenWidth / 2.0f - 150,  screenHeight / 2.0f - 200, 300, 40, "IP (ex: 127.0.0.1)", LIGHTGRAY);
    connectButton = initIaCElement(screenWidth / 2.0f - 100,  screenHeight / 2.0f - 125, 200, 40, "Se Connecter", DARKGRAY);
    
    // Bouton pour lancer le broadcast de découverte
    refreshButton = initIaCElement((float)(screenWidth - 180), (screenHeight / 2.0f - 40), 160, 30, "Rafraichir", SKYBLUE);

    for (int i = 0; i < MAX_ROOMS_DISPLAY; i++) discoveredRooms[i].active = false;
    roomsCount = 0;
}

/**
 * @brief Adds a discovered server to the internal list for display.
 * @param ip IP address of the discovered server.
 * @param name Name of the discovered server.
 */
void addDiscoveredRoom(const char* ip, const char* name) {
    if (roomsCount >= MAX_ROOMS_DISPLAY) return;
    
    for (int i = 0; i < roomsCount; i++) {
        if (strcmp(discoveredRooms[i].ip, ip) == 0) return;
    }

    strncpy(discoveredRooms[roomsCount].ip, ip, 15);
    strncpy(discoveredRooms[roomsCount].name, name, 31);
    
    float startY = (float)(GetScreenHeight() / 2.0f + 20);
    
    discoveredRooms[roomsCount].button = initIaCElement(
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

extern void discoverServers(void);

/**
 * @brief Logical update loop for the connection screen.
 * @return true if connection is triggered (valid IP + click), false otherwise.
 */
bool updateConnectionScreen(void) {
    updateIPInput(&ipInput, ipBuffer, &letterCount);

    if (updateConnectButton(&refreshButton, true)) {
        discoverServers(); 
    }

    for (int i = 0; i < roomsCount; i++) {
        if (updateConnectButton(&discoveredRooms[i].button, true)) {
            strncpy(ipBuffer, discoveredRooms[i].ip, IP_MAX_LENGTH);
            letterCount = (int) strlen(ipBuffer);
            ipInput.isIPValid = true;
        }
    }

    return updateConnectButton(&connectButton, ipInput.isIPValid);
}

/**
 * @brief Renders the connection screen UI.
 */
void drawConnectionScreen(void) {
    ClearBackground(RAYWHITE);
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("CONNEXION AU SERVEUR", sw/2 - MeasureText("CONNEXION AU SERVEUR", 30)/2, sh/2 - 250, 30, DARKGRAY);

    drawIaCElement(ipInput, ipBuffer);
    drawIaCElement(connectButton, "");
    drawIaCElement(refreshButton, "");

    if (!ipInput.isIPValid && ipInput.state == STATE_ACTIVE) {
        DrawText("Format attendu : XXX.XXX.XXX.XXX", sw/2 - 100, sh/2 - 150, 15, RED);
    }

    DrawText("Liste des rooms :", 20, sh/2 - 30, 25, DARKBLUE);
    DrawRectangle(20, sh / 2.0f, sw - 40, 275, LIGHTGRAY);
    DrawRectangleLines(20, sh / 2.0f, sw - 40, 275, GRAY);

    for (int i = 0; i < roomsCount; i++) {
        drawIaCElement(discoveredRooms[i].button, discoveredRooms[i].name);
        DrawText(discoveredRooms[i].ip, sw - 180, (int)discoveredRooms[i].button.rect.y + 10, 18, GRAY);
    }
    
    if (roomsCount == 0) {
        DrawText("Aucun serveur détecté. Cliquez sur Rafraichir.", sw/2 - 150, sh/2 + 100, 15, DARKGRAY);
    }
}

/**
 * @brief Gets the IP address entered in the input field.
 * @return A pointer to the IP buffer string.
 */
const char* getEnteredIP(void) { return ipBuffer; }