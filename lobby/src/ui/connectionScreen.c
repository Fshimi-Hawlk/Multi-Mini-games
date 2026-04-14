/**
    @file ui/connectionScreen.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-04-13
    @date 2026-04-13
    @brief Two-layer connection screen (Server List → Room List) using shared widgets.

    Contributors:
        - i-Charlys: Original discovery logic
        - Fshimi-Hawlk: Two-layer architecture, player name, instance support,
                        full widget migration, textColor fix, lobby_ prefix
*/
#include "ui/connectionScreen.h"

#include "utils/globals.h"

#include "sharedWidgets/textBox.h"
#include "sharedWidgets/button.h"
#include "sharedWidgets/types.h"

#define MAX_SERVERS_DISPLAY 8
#define MAX_INSTANCES_DISPLAY 8

typedef struct {
    char          ip[16];
    char          name[32];
    TextButton_St button;
    bool          active;
} ServerEntry_St;

static TextBox_St    lobby_playerNameTextBox;
static TextBox_St    lobby_ipTextBox;
static TextButton_St lobby_refreshButton;
static TextButton_St lobby_connectButton;

static ServerEntry_St lobby_discoveredServers[MAX_SERVERS_DISPLAY];
static s32            lobby_serversCount = 0;

static char           lobby_errorMessage[128] = "";
static f32            lobby_errorTimer = 0.0f;

// static bool           lobby_playerInfoOverlayOpen = false;
// static Texture2D      lobby_playerIconTexture;   // initialized in lobby startup

static bool isValidIPv4(const char* ip) {
    if (ip == NULL || ip[0] == '\0') return false;
    s32 octet = 0, num = 0, dots = 0;
    bool hasDigit = false;
    for (s32 i = 0; ip[i]; ++i) {
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

void lobby_initConnectionScreen(void) {
    s32 w = systemSettings.video.width;
    s32 h = systemSettings.video.height;
    f32 centerX = (f32)w / 2.0f;
    f32 titleY  = (f32)h * 0.09f;

    lobby_playerNameTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 80.0f, 370.0f, 46.0f},
        .state       = WIDGET_STATE_NORMAL,
        .roundness   = 0.4f,
        .placeholder = "Enter your player name",
        .isValid     = true
    };
    strncpy(lobby_playerNameTextBox.buffer, "unnamed", 31);

    lobby_refreshButton = (TextButton_St) {
        .bounds    = {w - 90.0f - 135.0f, titleY + 220.0f, 170.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color){80, 180, 255, 255},
        .roundness = 0.4f,
        .text      = "Refresh",
        .textColor = WHITE
    };

    lobby_ipTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 145.0f, 370.0f, 50.0f},
        .state       = WIDGET_STATE_NORMAL,
        .roundness   = 0.4f,
        .placeholder = "Enter server IP",
        .isValid     = false
    };

    lobby_connectButton = (TextButton_St) {
        .bounds    = {centerX - 105.0f, titleY + 210.0f, 210.0f, 50.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color){70, 130, 255, 255},
        .roundness = 0.4f,
        .text      = "Connect",
        .textColor = WHITE
    };

    for (s32 i = 0; i < MAX_SERVERS_DISPLAY; ++i) lobby_discoveredServers[i].active = false;
    lobby_serversCount = 0;
}

void lobby_addDiscoveredServer(const char* ip, const char* name) {
    if (lobby_serversCount >= MAX_SERVERS_DISPLAY) return;
    for (s32 i = 0; i < lobby_serversCount; ++i) {
        if (strcmp(lobby_discoveredServers[i].ip, ip) == 0) return;
    }

    strncpy(lobby_discoveredServers[lobby_serversCount].ip, ip, 15);
    strncpy(lobby_discoveredServers[lobby_serversCount].name, name, 31);

    f32 listStartY = systemSettings.video.height * 0.09f + 285.0f;

    lobby_discoveredServers[lobby_serversCount].button = (TextButton_St) {
        .bounds    = {55.0f, listStartY + (lobby_serversCount * 52.0f), systemSettings.video.width - 110.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .baseColor = (Color){55, 55, 70, 255},
        .roundness = 0.4f,
        .text      = lobby_discoveredServers[lobby_serversCount].name,
        .textColor = WHITE
    };
    lobby_discoveredServers[lobby_serversCount].active = true;
    ++lobby_serversCount;
}

extern void discoverServers(void);

bool lobby_updateConnectionScreen(void) {
    if (lobby_errorTimer > 0.0f) lobby_errorTimer -= GetFrameTime();
    else lobby_errorMessage[0] = '\0';

    Vector2 m = GetMousePosition();

    textBoxUpdate(&lobby_playerNameTextBox, m);
    textBoxUpdate(&lobby_ipTextBox, m);
    lobby_ipTextBox.isValid = isValidIPv4(lobby_ipTextBox.buffer);

    if (textButtonUpdate(&lobby_refreshButton, m)) discoverServers();

    for (s32 i = 0; i < lobby_serversCount; ++i) {
        if (lobby_discoveredServers[i].active && textButtonUpdate(&lobby_discoveredServers[i].button, m)) {
            strncpy(lobby_ipTextBox.buffer, lobby_discoveredServers[i].ip, 15);
            lobby_ipTextBox.cursorPos = (s32)strlen(lobby_ipTextBox.buffer);
            lobby_ipTextBox.isValid = true;
        }
    }

    bool ipValid = lobby_ipTextBox.isValid;
    if (lobby_connectButton.state != WIDGET_STATE_CLICK) {
        lobby_connectButton.state = ipValid ? WIDGET_STATE_NORMAL : WIDGET_STATE_DISABLED;
    }

    if (textButtonUpdate(&lobby_connectButton, m) && ipValid) return true;
    return false;
}

void lobby_drawConnectionScreen(void) {
    ClearBackground((Color){18, 18, 24, 255});
    s32 w = systemSettings.video.width;
    s32 h = systemSettings.video.height;
    f32 cX = (f32)w / 2.0f;
    f32 tY = (f32)h * 0.09f;

    DrawTextEx(
        lobby_fonts[FONT48], "CONNECT TO SERVER",
        (Vector2){cX - MeasureTextEx(lobby_fonts[FONT48], "CONNECT TO SERVER", 48, 2).x / 2.0f, tY},
        48, 2, (Color){235, 235, 255, 255}
    );

    if (lobby_errorMessage[0] != '\0') {
        DrawTextEx(
            lobby_fonts[FONT24], lobby_errorMessage,
            (Vector2){cX - MeasureTextEx(lobby_fonts[FONT24], lobby_errorMessage, 24, 0).x / 2.0f, tY + 60.0f},
            24, 0, RED
        );
    }

    Font uiF = lobby_fonts[FONT24];
    textBoxDraw(&lobby_playerNameTextBox, uiF, 24.0f);
    textBoxDraw(&lobby_ipTextBox, uiF, 24.0f);
    textButtonDraw(&lobby_refreshButton, uiF, 24.0f);
    textButtonDraw(&lobby_connectButton, uiF, 24.0f);

    f32 listY = tY + 275.0f;
    Rectangle listR = {45.0f, listY, (f32)w - 90.0f, 220.0f};
    DrawRectangleRec(listR, (Color){28, 28, 38, 255});
    DrawRectangleLinesEx(listR, 3.0f, (Color){75, 85, 115, 255});

    for (s32 i = 0; i < lobby_serversCount; ++i) {
        if (lobby_discoveredServers[i].active) {
            textButtonDraw(&lobby_discoveredServers[i].button, lobby_fonts[FONT24], 20.0f);
            DrawTextEx(
                lobby_fonts[FONT24], lobby_discoveredServers[i].ip,
                (Vector2){(f32)w - 215.0f, lobby_discoveredServers[i].button.bounds.y + 14.0f},
                18, 1, (Color){165, 175, 195, 255}
            );
        }
    }
}

void lobby_setConnectionError(const char* error) {
    if (error) {
        strncpy(lobby_errorMessage, error, 127);
        lobby_errorTimer = 5.0f;
    }
}

const char* lobby_getEnteredIP(void) { return lobby_ipTextBox.buffer; }
const char* lobby_getEnteredPseudo(void) { return lobby_playerNameTextBox.buffer; }