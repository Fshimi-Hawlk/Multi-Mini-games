/**
    @file ui/connectionScreen.c
    @author Fshimi-Hawlk
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-04-04
    @brief Two-layer modern connection screen implementing the new server/instance terminology.

    Layer 1 (Server List):
        - Player name input (sent on connect)
        - Discovered servers list (renamed from old "rooms")
        - Manual IP textbox with strict IPv4 validation
        - Refresh + Connect buttons

    Layer 2 (Room List - appears after successful server connection):
        - List of waiting game instances on the connected server
        - Lobby is always the first entry (shows total connected players)
        - Each row = one game instance (host name + game type + player count + Join button)

    @note All terminology updated: "server" = discovered remote machine, "room" = in-game instance.
          Old functions/variables have been renamed.
          Validation, roundness, placeholder and live border feedback are preserved.

    Contributors:
        - i-Charlys (CAILLON Charles):
            - Original implementation
        - Fshimi-Hawlk:
            - Two-layer architecture, player name input, terminology rename (room→server),
              instance list UI, widget integration
*/

#include "widgets/textBox.h"
#include "widgets/button.h"
#include "widgets/types.h"

#include "ui/connectionScreen.h"

#include "utils/globals.h"
#include "systemSettings.h"
#include "APIs/generalAPI.h"
#include "networkInterface.h"

#define MAX_SERVERS_DISPLAY 8     ///< Maximum number of discovered servers shown at once.
#define MAX_INSTANCES_DISPLAY 8   ///< Maximum number of game instances shown in Layer 2.

/**
    @brief One discovered server entry (Layer 1).
*/
typedef struct {
    char          ip[16];
    char          name[64];
    TextButton_St button;
    bool          active;
} ServerEntry_St;

/**
    @brief One game instance entry (Layer 2 - Room List).
*/
typedef struct {
    u32           instanceId;
    MiniGame_Et   gameType;
    char          hostName[32];
    u8            playerCount;
    u8            maxPlayers;
    TextButton_St joinButton;
    bool          active;
} InstanceEntry_St;

static TextBox_St    playerNameTextBox;
static TextBox_St    ipTextBox;
static TextButton_St refreshButton;
static TextButton_St connectButton;

static ServerEntry_St   discoveredServers[MAX_SERVERS_DISPLAY];
static int              serversCount = 0;

static InstanceEntry_St gameInstances[MAX_INSTANCES_DISPLAY];
static int              instancesCount = 0;

static ConnectionLayer_Et currentLayer = CONNECTION_LAYER_SERVER_LIST;

/**
    @brief Mapping from MiniGame_Et to human-readable name (used in Layer 2).
*/
static const char* const gameTypeNames[__miniGameCount] = {
    [MINI_GAME_LOBBY] = "Lobby",
    [MINI_GAME_BINGO] = "Bingo",
    [MINI_GAME_KFF]   = "King-for-Four",
    // Add new games here when integrated
};

/**
    @brief Strict IPv4 validation.
*/
static bool isValidIPv4(const char* ip) {
    if (ip == NULL || ip[0] == '\0') return false;

    int octet = 0;
    int num   = 0;
    int dots  = 0;
    bool hasDigit = false;

    for (int i = 0; ip[i]; ++i) {
        char c = ip[i];

        if (c == '.') {
            if (!hasDigit || octet > 255 || dots >= 3) return false;
            dots++;
            octet = 0;
            num = 0;
            hasDigit = false;
            continue;
        }

        if (c < '0' || c > '9') return false;

        hasDigit = true;
        octet = octet * 10 + (c - '0');
        if (octet > 255) return false;

        if (num == 0 && c == '0' && ip[i+1] != '.' && ip[i+1] != '\0') {
            if (ip[i+1] >= '0' && ip[i+1] <= '9') return false;
        }
        num++;
    }

    return (dots == 3 && hasDigit && octet <= 255);
}

void initConnectionScreen(void) {
    int w = systemSettings.video.width;
    int h = systemSettings.video.height;

    float centerX = (float)w / 2.0f;
    float titleY  = (float)h * 0.09f;

    playerNameTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 40.0f, 370.0f, 46.0f},
        .state       = WIDGET_STATE_NORMAL,
        .buffer[0]   = '\0',
        .cursorPos   = 0,
        .editMode    = false,
        .roundness   = 0.4f,
        .placeholder = "Enter your player name (e.g. Player42)",
        .isValid     = true
    };

    refreshButton = (TextButton_St) {
        .bounds    = {w - 90.0f - 135.0f, titleY + 220.0f, 170.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "Refresh",
        .baseColor = (Color) {80, 180, 255, 255},
        .roundness = 0.4f
    };

    ipTextBox = (TextBox_St) {
        .bounds      = {centerX - 185.0f, titleY + 105.0f, 370.0f, 50.0f},
        .state       = WIDGET_STATE_NORMAL,
        .buffer[0]   = '\0',
        .cursorPos   = 0,
        .editMode    = false,
        .roundness   = 0.4f,
        .placeholder = "Enter server IP (e.g. 127.0.0.1)",
        .isValid     = false
    };

    connectButton = (TextButton_St) {
        .bounds    = {centerX - 105.0f, titleY + 170.0f, 210.0f, 50.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "Connect",
        .baseColor = (Color) {70, 130, 255, 255},
        .roundness = 0.4f
    };

    for (int i = 0; i < MAX_SERVERS_DISPLAY; ++i) {
        discoveredServers[i].active = false;
    }
    
    serversCount = 0;

    for (int i = 0; i < MAX_INSTANCES_DISPLAY; ++i) {
        gameInstances[i].active = false;
    }

    instancesCount = 0;

    currentLayer = CONNECTION_LAYER_SERVER_LIST;

    strncpy(playerNameTextBox.buffer, "unnamed", sizeof(playerNameTextBox.buffer) - 1);
    playerNameTextBox.cursorPos = (u32)strlen(playerNameTextBox.buffer);
}

void addDiscoveredServer(const char* ip, const char* name) {
    if (serversCount >= MAX_SERVERS_DISPLAY) return;

    for (int i = 0; i < serversCount; ++i) {
        if (strcmp(discoveredServers[i].ip, ip) == 0) return;
    }

    strncpy(discoveredServers[serversCount].ip, ip, 15);
    strncpy(discoveredServers[serversCount].name, name, 63);

    float listStartY = systemSettings.video.height * 0.09f + 285.0f;

    discoveredServers[serversCount].button = (TextButton_St) {
        .bounds    = {55.0f, listStartY + (serversCount * 52.0f), systemSettings.video.width - 110.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = discoveredServers[serversCount].name,
        .baseColor = (Color) {55, 55, 70, 255},
        .roundness = 0.4f
    };

    discoveredServers[serversCount].active = true;
    ++serversCount;
}

void addGameInstance(u32 instanceId, MiniGame_Et gameType, const char* hostName,
                     u8 playerCount, u8 maxPlayers) {
    if (instancesCount >= MAX_INSTANCES_DISPLAY) return;

    for (int i = 0; i < instancesCount; ++i) {
        if (gameInstances[i].instanceId == instanceId) return;
    }

    gameInstances[instancesCount].instanceId   = instanceId;
    gameInstances[instancesCount].gameType     = gameType;
    gameInstances[instancesCount].playerCount  = playerCount;
    gameInstances[instancesCount].maxPlayers   = maxPlayers;
    strncpy(gameInstances[instancesCount].hostName, hostName, 31);

    float listStartY = systemSettings.video.height * 0.09f + 220.0f;

    gameInstances[instancesCount].joinButton = (TextButton_St) {
        .bounds    = {55.0f, listStartY + (instancesCount * 52.0f), systemSettings.video.width - 110.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "Join",
        .baseColor = (Color) {70, 130, 255, 255},
        .roundness = 0.4f
    };

    gameInstances[instancesCount].active = true;
    ++instancesCount;
}

extern void discoverServers(void);
extern void requestInstanceList(void);

bool updateConnectionScreen(void) {
    Vector2 mouseScreen = GetMousePosition();

    if (currentLayer == CONNECTION_LAYER_SERVER_LIST) {
        textBoxUpdate(&playerNameTextBox, mouseScreen);
    }

    textBoxUpdate(&ipTextBox, mouseScreen);
    ipTextBox.isValid = isValidIPv4(ipTextBox.buffer);

    if (textButtonUpdate(&refreshButton, mouseScreen)) {
        if (currentLayer == CONNECTION_LAYER_SERVER_LIST) {
            discoverServers();
        } else {
            requestInstanceList();
        }
    }

    if (currentLayer == CONNECTION_LAYER_SERVER_LIST) {
        for (int i = 0; i < serversCount; ++i) {
            if (discoveredServers[i].active && textButtonUpdate(&discoveredServers[i].button, mouseScreen)) {
                strncpy(ipTextBox.buffer, discoveredServers[i].ip, sizeof(ipTextBox.buffer) - 1);
                ipTextBox.buffer[sizeof(ipTextBox.buffer) - 1] = '\0';
                ipTextBox.cursorPos = (u32)strlen(ipTextBox.buffer);
                ipTextBox.isValid = true;
            }
        }
    }

    if (currentLayer == CONNECTION_LAYER_ROOM_LIST) {
        for (int i = 0; i < instancesCount; ++i) {
            if (gameInstances[i].active && textButtonUpdate(&gameInstances[i].joinButton, mouseScreen)) {
                // Send JOIN_INSTANCE packet immediately
                RUDPHeader_St h;
                rudpGenerateHeader(&serverConnection, ACTION_CODE_JOIN_INSTANCE, &h);

                u8 buffer[sizeof(RUDPHeader_St) + sizeof(u32)];
                memcpy(buffer, &h, sizeof(h));
                *(u32*)(buffer + sizeof(h)) = gameInstances[i].instanceId;

                send(networkSocket, buffer, sizeof(buffer), 0);
                return true;
            }
        }
    }

    bool ipValid = ipTextBox.isValid;
    WidgetState_Et desiredState = ipValid ? WIDGET_STATE_NORMAL : WIDGET_STATE_DISABLED;

    if (connectButton.state != WIDGET_STATE_CLICK) {
        connectButton.state = desiredState;
    }

    bool clicked = textButtonUpdate(&connectButton, mouseScreen);

    if (clicked && ipValid && currentLayer == CONNECTION_LAYER_SERVER_LIST) {
        currentLayer = CONNECTION_LAYER_ROOM_LIST;
        return true;
    }

    return false;
}

void drawConnectionScreen(void) {
    ClearBackground((Color) {18, 18, 24, 255});

    int w = systemSettings.video.width;
    int h = systemSettings.video.height;
    float centerX = (float)w / 2.0f;
    float titleY  = (float)h * 0.09f;

    if (currentLayer == CONNECTION_LAYER_SERVER_LIST) {
        const char* title = "CONNECT TO SERVER";
        Vector2 titleSize = MeasureTextEx(lobby_fonts[FONT48], title, 48, 2);
        DrawTextEx(
            lobby_fonts[FONT48], title,
            (Vector2){centerX - titleSize.x / 2.0f, titleY},
            48, 2, (Color){235, 235, 255, 255}
        );

        DrawTextEx(
            lobby_fonts[FONT20], "Enter your player name and connect to a server",
            (Vector2){centerX - 220.0f, titleY + 55.0f},
            20, 1, (Color){180, 180, 200, 255}
        );

        textBoxDraw(&playerNameTextBox, lobby_fonts[FONT24], 24.0f);
        textBoxDraw(&ipTextBox, lobby_fonts[FONT24], 24.0f);
        textButtonDraw(&refreshButton, lobby_fonts[FONT24], 24.0f);
        textButtonDraw(&connectButton, lobby_fonts[FONT24], 24.0f);

        float detectedY = titleY + 245.0f;
        DrawTextEx(
            lobby_fonts[FONT28], "Detected Servers",
            (Vector2){48.0f, detectedY}, 28, 1, (Color){200, 220, 255, 255}
        );

        Rectangle listRect = {45.0f, detectedY + 30.0f, (float)w - 90.0f, 220.0f};
        DrawRectangleRec(listRect, (Color){28, 28, 38, 255});
        DrawRectangleLinesEx(listRect, 3.0f, (Color){75, 85, 115, 255});

        for (int i = 0; i < serversCount; ++i) {
            if (discoveredServers[i].active) {
                textButtonDraw(&discoveredServers[i].button, lobby_fonts[FONT20], 20.0f);

                float ipTextY = discoveredServers[i].button.bounds.y +
                                (discoveredServers[i].button.bounds.height - 18.0f) / 2.0f + 1.0f;
                DrawTextEx(
                    lobby_fonts[FONT18], discoveredServers[i].ip,
                    (Vector2){(float)w - 215.0f, ipTextY},
                    18, 1, (Color){165, 175, 195, 255}
                );
            }
        }

        if (serversCount == 0) {
            DrawTextEx(
                lobby_fonts[FONT20],
                "No servers detected. Click Refresh.",
                (Vector2){centerX - 185.0f, detectedY + 115.0f},
                20, 1, (Color){145, 145, 165, 255}
            );
        }
    } else {
        const char* title = "AVAILABLE GAME ROOMS";
        Vector2 titleSize = MeasureTextEx(lobby_fonts[FONT48], title, 48, 2);
        DrawTextEx(
            lobby_fonts[FONT48], title,
            (Vector2){centerX - titleSize.x / 2.0f, titleY},
            48, 2, (Color){235, 235, 255, 255}
        );

        DrawTextEx(
            lobby_fonts[FONT20], "Choose a game room to join",
            (Vector2){centerX - 150.0f, titleY + 55.0f},
            20, 1, (Color){180, 180, 200, 255}
        );

        textButtonDraw(&refreshButton, lobby_fonts[FONT24], 24.0f);

        float listY = titleY + 120.0f;
        Rectangle listRect = {45.0f, listY, (float)w - 90.0f, 320.0f};
        DrawRectangleRec(listRect, (Color){28, 28, 38, 255});
        DrawRectangleLinesEx(listRect, 3.0f, (Color){75, 85, 115, 255});

        for (int i = 0; i < instancesCount; ++i) {
            if (gameInstances[i].active) {
                textButtonDraw(&gameInstances[i].joinButton, lobby_fonts[FONT20], 20.0f);

                char info[128];
                const char* gameName = (gameInstances[i].gameType < __miniGameCount)
                                     ? gameTypeNames[gameInstances[i].gameType]
                                     : "Unknown";

                snprintf(
                    info, sizeof(info), "%s  •  %s  •  %d/%d",
                    gameInstances[i].hostName, gameName,
                    gameInstances[i].playerCount, gameInstances[i].maxPlayers
                );

                DrawTextEx(
                    lobby_fonts[FONT20], info,
                    (Vector2){70.0f, gameInstances[i].joinButton.bounds.y + 14.0f},
                    20, 1, (Color){235, 235, 255, 255}
                );
            }
        }

        if (instancesCount == 0) {
            DrawTextEx(
                lobby_fonts[FONT20],
                "No game rooms available yet.\nClick Refresh or create a new one.",
                (Vector2){centerX - 220.0f, listY + 120.0f},
                20, 1, (Color){145, 145, 165, 255}
            );
        }
    }
}

const char *getEnteredIP(void) {
    return ipTextBox.buffer;
}

const char *getPlayerName(void) {
    return playerNameTextBox.buffer;
}

void switchToRoomListLayer(void) {
    currentLayer = CONNECTION_LAYER_ROOM_LIST;
    instancesCount = 0;
}

bool isInRoomListLayer(void) {
    return currentLayer == CONNECTION_LAYER_ROOM_LIST;
}