/**
    @file ui/connection_screen.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-30
    @brief Modern connection screen using the enhanced reusable widget system.
*/

#include "ui/connection_screen.h"
#include "widgets/textBox.h"
#include "widgets/button.h"
#include "widgets/types.h"
#include "utils/globals.h"
#include "systemSettings.h"
#include <string.h>
#include <stdio.h>

#define MAX_ROOMS_DISPLAY 5

typedef struct {
    char          ip[16];
    char          name[32];
    TextButton_St button;
    bool          active;
} RoomEntry_St;

static TextBox_St    ipTextBox;
static TextBox_St    pseudoTextBox;
static TextButton_St refreshButton;
static TextButton_St connectButton;

static RoomEntry_St discoveredRooms[MAX_ROOMS_DISPLAY];
static int          roomsCount = 0;

static bool isValidIPv4(const char* ip) {
    if (ip == NULL || ip[0] == '\0') return false;
    int octet = 0, num = 0, dots = 0;
    bool hasDigit = false;
    for (int i = 0; ip[i]; ++i) {
        char c = ip[i];
        if (c == '.') {
            if (!hasDigit || octet > 255 || dots >= 3) return false;
            dots++; octet = 0; num = 0; hasDigit = false; continue;
        }
        if (c < '0' || c > '9') return false;
        hasDigit = true;
        octet = octet * 10 + (c - '0');
        if (octet > 255) return false;
        if (num == 0 && c == '0' && ip[i+1] != '.' && ip[i+1] != '\0') return false;
        num++;
    }
    return (dots == 3 && hasDigit && octet <= 255);
}

void initConnectionScreen(void) {
    int w = systemSettings.video.width;
    int h = systemSettings.video.height;
    float centerX = (float)w / 2.0f;
    float titleY  = (float)h * 0.09f;

    refreshButton = (TextButton_St) {
        .bounds    = {w - 90.0f - 135.0f, titleY + 220.0f, 170.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color) {80, 180, 255, 255},
        .roundness = 0.4f
    };
    strncpy(refreshButton.text, "Refresh", 63);

    ipTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 105.0f, 370.0f, 50.0f},
        .state       = WIDGET_STATE_NORMAL,
        .cursorPos   = 0,
        .editMode    = false,
        .roundness   = 0.4f,
        .placeholder = "Enter server IP",
        .isValid     = false
    };
    ipTextBox.buffer[0] = '\0';

    pseudoTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 40.0f, 370.0f, 50.0f},
        .state       = WIDGET_STATE_NORMAL,
        .cursorPos   = 0,
        .editMode    = false,
        .roundness   = 0.4f,
        .placeholder = "Enter your Pseudo",
        .isValid     = true
    };
    strncpy(pseudoTextBox.buffer, "Joueur", 31);
    pseudoTextBox.cursorPos = 6;

    connectButton = (TextButton_St) {
        .bounds    = {centerX - 105.0f, titleY + 170.0f, 210.0f, 50.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color) {70, 130, 255, 255},
        .roundness = 0.4f
    };
    strncpy(connectButton.text, "Connect", 63);

    for (int i = 0; i < MAX_ROOMS_DISPLAY; ++i) discoveredRooms[i].active = false;
    roomsCount = 0;
}

void addDiscoveredRoom(const char* ip, const char* name) {
    if (roomsCount >= MAX_ROOMS_DISPLAY) return;
    for (int i = 0; i < roomsCount; ++i) if (strcmp(discoveredRooms[i].ip, ip) == 0) return;
    strncpy(discoveredRooms[roomsCount].ip, ip, 15);
    strncpy(discoveredRooms[roomsCount].name, name, 31);
    float listStartY = systemSettings.video.height * 0.09f + 285.0f;
    discoveredRooms[roomsCount].button = (TextButton_St) {
        .bounds    = {55.0f, listStartY + (roomsCount * 52.0f), systemSettings.video.width - 110.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color) {55, 55, 70, 255},
        .roundness = 0.4f
    };
    strncpy(discoveredRooms[roomsCount].button.text, discoveredRooms[roomsCount].name, 63);
    discoveredRooms[roomsCount].active = true;
    ++roomsCount;
}

extern void discoverServers(void);

bool updateConnectionScreen(void) {
    Vector2 m = GetMousePosition();
    textBoxUpdate(&ipTextBox, m);
    textBoxUpdate(&pseudoTextBox, m);
    ipTextBox.isValid = isValidIPv4(ipTextBox.buffer);
    if (textButtonUpdate(&refreshButton, m)) discoverServers();
    for (int i = 0; i < roomsCount; i++) {
        if (discoveredRooms[i].active && textButtonUpdate(&discoveredRooms[i].button, m)) {
            strncpy(ipTextBox.buffer, discoveredRooms[i].ip, 15);
            ipTextBox.cursorPos = strlen(ipTextBox.buffer);
            ipTextBox.isValid = true;
        }
    }
    bool ipValid = ipTextBox.isValid;
    if (connectButton.state != WIDGET_STATE_CLICK) connectButton.state = ipValid ? WIDGET_STATE_NORMAL : WIDGET_STATE_DISABLED;
    if (textButtonUpdate(&connectButton, m) && ipValid) return true;
    return false;
}

void drawConnectionScreen(void) {
    ClearBackground((Color) {18, 18, 24, 255});
    int w = systemSettings.video.width;
    int h = systemSettings.video.height;
    float cX = (float)w / 2.0f;
    float tY = (float)h * 0.09f;
    DrawTextEx(lobby_fonts[FONT48], "CONNECT TO SERVER", (Vector2) {cX - MeasureTextEx(lobby_fonts[FONT48], "CONNECT TO SERVER", 48, 2).x / 2.0f, tY}, 48, 2, (Color) {235, 235, 255, 255});
    Font uiF = lobby_fonts[FONT24];
    textBoxDraw(&ipTextBox, uiF, 24.0f);
    textBoxDraw(&pseudoTextBox, uiF, 24.0f);
    textButtonDraw(&refreshButton, uiF, 24.0f);
    textButtonDraw(&connectButton, uiF, 24.0f);
    float listY = tY + 275.0f;
    Rectangle listR = { 45.0f, listY, (float)w - 90.0f, 220.0f };
    DrawRectangleRec(listR, (Color) {28, 28, 38, 255});
    DrawRectangleLinesEx(listR, 3.0f, (Color) {75, 85, 115, 255});
    for (int i = 0; i < roomsCount; i++) {
        if (discoveredRooms[i].active) {
            textButtonDraw(&discoveredRooms[i].button, lobby_fonts[FONT20], 20.0f);
            DrawTextEx(lobby_fonts[FONT18], discoveredRooms[i].ip, (Vector2) {(float)w - 215.0f, discoveredRooms[i].button.bounds.y + 14.0f}, 18, 1, (Color) {165, 175, 195, 255});
        }
    }
}

const char* getEnteredIP(void) { return ipTextBox.buffer; }
const char* getEnteredPseudo(void) { return pseudoTextBox.buffer; }
