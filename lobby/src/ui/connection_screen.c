/**
<<<<<<< HEAD
 * @file connection_screen.c
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Implementation of the connection screen for server discovery and IP entry.
 */
=======
    @file ui/connection_screen.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-18
    @date 2026-03-30
    @brief Modern connection screen using the enhanced reusable widget system.
>>>>>>> origin/mgit-PR1-20-03

    Features:
      • Rounded corners on all widgets (TextBox + TextButton)
      • IP TextBox with placeholder text when empty
      • Proper IPv4 validation (each octet 0-255, exactly 3 dots, digits only)
      • Live border feedback: green when valid, red when invalid (while active)
      • Refresh button placed directly above the room list container
      • Room entry buttons span the full width of the list panel
      • Clear disabled state for Connect button (darker background + gray text)
      • Clicking a discovered room copies its IP into the textbox and marks it valid

    @note Validation uses a strict manual parser (no <regex.h> dependency for portability).
          It rejects leading zeros in octets except for 0 itself and enforces 0-255 range.

    Contributors:
        - i-Charlys (CAILLON Charles):
            - Original implementation
        - Fshimi-Hawlk:
            - Full widget refactor with roundness, placeholder, validation coloring
            - Strict IPv4 parser + UI layout improvements
*/

#include "ui/connection_screen.h"

#include "widgets/textBox.h"
#include "widgets/button.h"
#include "widgets/types.h"

#include "utils/globals.h"

#include "systemSettings.h"

#define MAX_ROOMS_DISPLAY 5     ///< Maximum number of discovered rooms shown at once.

/**
    @brief One discovered room entry in the UI list.
*/
typedef struct {
    char          ip[16];
    char          name[32];
    TextButton_St button;
    bool          active;
} RoomEntry_St;

<<<<<<< HEAD
/** @brief UI input for the IP address. */
static IaC_button ipInput;
/** @brief UI input for the nickname. */
static IaC_button pseudoInput;
/** @brief UI button to trigger connection. */
static IaC_button connectButton;
/** @brief UI button to refresh the server list. */
static IaC_button refreshButton;
/** @brief Buffer for the entered IP address. */
static char ipBuffer[IP_MAX_LENGTH + 1] = {0};
/** @brief Current character count in the IP buffer. */
static int letterCount = 0;
/** @brief Buffer for the entered nickname. */
static char pseudoBuffer[32] = "Joueur";
/** @brief Current character count in the nickname buffer. */
static int pseudoLetterCount = 6;
=======
static TextBox_St    ipTextBox;
static TextButton_St refreshButton;
static TextButton_St connectButton;
>>>>>>> origin/mgit-PR1-20-03

static RoomEntry_St discoveredRooms[MAX_ROOMS_DISPLAY];
static int          roomsCount = 0;

/**
<<<<<<< HEAD
 * @brief Allocates and positions UI elements for the connection screen.
 */
void initConnectionScreen(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    ipInput = InitIaCElement(screenWidth / 2.0f - 150,  screenHeight / 2.0f - 200, 300, 40, "IP (ex: 127.0.0.1)", LIGHTGRAY);
    pseudoInput = InitIaCElement(screenWidth / 2.0f - 150, screenHeight / 2.0f - 260, 300, 40, "Pseudo", LIGHTGRAY);
    connectButton = InitIaCElement(screenWidth / 2.0f - 100,  screenHeight / 2.0f - 125, 200, 40, "Se Connecter", DARKGRAY);
    
    // Bouton pour lancer le broadcast de découverte
    refreshButton = InitIaCElement((float)(screenWidth - 180), (screenHeight / 2.0f - 40), 160, 30, "Rafraichir", SKYBLUE);
=======
    @brief Strict IPv4 validation: each octet must be 0-255, exactly 3 dots, digits only.
           Rejects leading zeros except for the number 0 itself.
    @param ip Null-terminated string to validate
    @return true if the string is a valid IPv4 address (e.g. "192.168.1.1", "10.0.0.0")
*/
static bool isValidIPv4(const char* ip) {
    if (ip == NULL || ip[0] == '\0') return false;

    int octet = 0;
    int num   = 0;
    int dots  = 0;
    bool hasDigit = false;
>>>>>>> origin/mgit-PR1-20-03

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

        // Reject leading zeros (except for the number 0)
        if (num == 0 && c == '0' && ip[i+1] != '.' && ip[i+1] != '\0') {
            // only allow single '0' per octet
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

    // Refresh button – positioned directly above the room list container
    refreshButton = (TextButton_St) {
        .bounds    = {w - 90.0f - 135.0f, titleY + 220.0f, 170.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "Refresh",
        .baseColor = (Color) {80, 180, 255, 255},
        .roundness = 0.4f
    };

    // IP input TextBox – modern rounded look with placeholder and validation feedback
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

    // Connect button
    connectButton = (TextButton_St) {
        .bounds    = {centerX - 105.0f, titleY + 170.0f, 210.0f, 50.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "Connect",
        .baseColor = (Color) {70, 130, 255, 255},
        .roundness = 0.4f
    };

    for (int i = 0; i < MAX_ROOMS_DISPLAY; ++i) {
        discoveredRooms[i].active = false;
    }
    roomsCount = 0;
}

<<<<<<< HEAD
/**
 * @brief Adds a discovered server to the internal list for display.
 * @param ip IP address of the discovered server.
 * @param name Name of the discovered server.
 */
=======
>>>>>>> origin/mgit-PR1-20-03
void addDiscoveredRoom(const char* ip, const char* name) {
    if (roomsCount >= MAX_ROOMS_DISPLAY) return;

    for (int i = 0; i < roomsCount; ++i) {
        if (strcmp(discoveredRooms[i].ip, ip) == 0) return;
    }

    strncpy(discoveredRooms[roomsCount].ip, ip, 15);
    discoveredRooms[roomsCount].ip[15] = '\0';
    strncpy(discoveredRooms[roomsCount].name, name, 31);
<<<<<<< HEAD
    discoveredRooms[roomsCount].name[31] = '\0';
    
    float startY = (float)(GetScreenHeight() / 2.0f + 20);
    
    discoveredRooms[roomsCount].button = InitIaCElement(
        30, 
        startY + (roomsCount * 50), 
        (float)GetScreenWidth() - 60, 
        40, 
        (char*)name, 
        WHITE
    );
    
=======

    float listStartY = systemSettings.video.height * 0.09f + 285.0f;

    // Room buttons span full container width
    discoveredRooms[roomsCount].button = (TextButton_St) {
        .bounds    = {55.0f, listStartY + (roomsCount * 52.0f), systemSettings.video.width - 110.0f, 46.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = discoveredRooms[roomsCount].name,
        .baseColor = (Color) {55, 55, 70, 255},
        .roundness = 0.4f
    };

>>>>>>> origin/mgit-PR1-20-03
    discoveredRooms[roomsCount].active = true;
    ++roomsCount;
}

extern void discoverServers(void);

<<<<<<< HEAD
/**
 * @brief Logical update loop for the connection screen.
 * @return true if connection is triggered (valid IP + click), false otherwise.
 */
bool updateConnectionScreen(void) {
    UpdateIPInput(&ipInput, ipBuffer, &letterCount);
    UpdateTextInput(&pseudoInput, pseudoBuffer, &pseudoLetterCount, 31);
=======
bool updateConnectionScreen(void) {
    Vector2 mouseScreen = GetMousePosition();
>>>>>>> origin/mgit-PR1-20-03

    textBoxUpdate(&ipTextBox, mouseScreen);

    // Live validation for visual feedback
    ipTextBox.isValid = isValidIPv4(ipTextBox.buffer);

    if (textButtonUpdate(&refreshButton, mouseScreen)) {
        discoverServers();
    }

    for (int i = 0; i < roomsCount; ++i) {
        if (discoveredRooms[i].active && textButtonUpdate(&discoveredRooms[i].button, mouseScreen)) {
            strncpy(ipTextBox.buffer, discoveredRooms[i].ip, sizeof(ipTextBox.buffer) - 1);
            ipTextBox.buffer[sizeof(ipTextBox.buffer) - 1] = '\0';
            ipTextBox.cursorPos = (u32)strlen(ipTextBox.buffer);
            ipTextBox.isValid = true;
        }
    }

    bool ipValid = ipTextBox.isValid;
    WidgetState_Et desiredState = ipValid ? WIDGET_STATE_NORMAL : WIDGET_STATE_DISABLED;

    // Only update the button state if it is not currently being clicked
    // This preserves the CLICK state so the release can be detected
    if (connectButton.state != WIDGET_STATE_CLICK) {
        connectButton.state = desiredState;
    }

    // Now let the widget handle input and detect release
    bool clicked = textButtonUpdate(&connectButton, mouseScreen);

    // Only accept the click if the IP is still valid at release time
    if (clicked && ipValid) {
        return true;
    }

    return false;
}

<<<<<<< HEAD
/**
 * @brief Renders the connection screen UI.
 */
void drawConnectionScreen(void) {
    ClearBackground(RAYWHITE);
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    DrawText("CONNEXION AU SERVEUR", sw/2 - MeasureText("CONNEXION AU SERVEUR", 30)/2, sh/2 - 320, 30, DARKGRAY);

    DrawIaCElement(pseudoInput, pseudoBuffer);
    DrawIaCElement(ipInput, ipBuffer);
    DrawIaCElement(connectButton, "");
    DrawIaCElement(refreshButton, "");
=======
void drawConnectionScreen(void) {
    ClearBackground((Color) {18, 18, 24, 255});

    int w = systemSettings.video.width;
    int h = systemSettings.video.height;
    float centerX = (float)w / 2.0f;
    float titleY  = (float)h * 0.09f;

    // Title
    const char* title = "CONNECT TO SERVER";
    Vector2 titleSize = MeasureTextEx(lobby_fonts[FONT48], title, 48, 2);
    DrawTextEx(lobby_fonts[FONT48], title,
               (Vector2) {centerX - titleSize.x / 2.0f, titleY},
               48, 2, (Color) {235, 235, 255, 255});
>>>>>>> origin/mgit-PR1-20-03

    // Subtitle
    DrawTextEx(lobby_fonts[FONT20], "Join a multiplayer session",
               (Vector2) {centerX - 150.0f, titleY + 55.0f},
               20, 1, (Color) {180, 180, 200, 255});

    Font uiFont   = lobby_fonts[FONT24];
    f32  fontSize = 24.0f;

    textBoxDraw(&ipTextBox, uiFont, fontSize);
    textButtonDraw(&refreshButton, uiFont, fontSize);
    textButtonDraw(&connectButton, uiFont, fontSize);

    // Detected Servers header
    float detectedY = titleY + 245.0f;
    DrawTextEx(lobby_fonts[FONT28], "Detected Servers",
               (Vector2) {48.0f, detectedY}, 28, 1, (Color) {200, 220, 255, 255});

    // Room list container
    float listY = titleY + 275.0f;
    Rectangle listRect = {
        45.0f,
        listY,
        (float)w - 90.0f,
        220.0f
    };
    DrawRectangleRec(listRect, (Color) {28, 28, 38, 255});
    DrawRectangleLinesEx(listRect, 3.0f, (Color) {75, 85, 115, 255});

    // Room entries
    for (int i = 0; i < roomsCount; ++i) {
        if (discoveredRooms[i].active) {
            textButtonDraw(&discoveredRooms[i].button, lobby_fonts[FONT20], 20.0f);

            float ipTextY = discoveredRooms[i].button.bounds.y +
                            (discoveredRooms[i].button.bounds.height - 18.0f) / 2.0f + 1.0f;
            DrawTextEx(lobby_fonts[FONT18], discoveredRooms[i].ip,
                       (Vector2) {(float)w - 215.0f, ipTextY},
                       18, 1, (Color) {165, 175, 195, 255});
        }
    }

    if (roomsCount == 0) {
        DrawTextEx(lobby_fonts[FONT20],
                   "No servers detected. Click Refresh.",
                   (Vector2) {centerX - 185.0f, listY + 85.0f},
                   20, 1, (Color) {145, 145, 165, 255});
    }
}

<<<<<<< HEAD
/**
 * @brief Gets the IP address entered in the input field.
 * @return A pointer to the IP buffer string.
 */
const char* getEnteredIP(void) { return ipBuffer; }

/**
 * @brief Gets the nickname entered in the input field.
 * @return A pointer to the nickname buffer string.
 */
const char* getEnteredPseudo(void) { return pseudoBuffer; }
=======
const char *getEnteredIP(void) {
    return ipTextBox.buffer;
}
>>>>>>> origin/mgit-PR1-20-03
