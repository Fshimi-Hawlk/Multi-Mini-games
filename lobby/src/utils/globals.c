#include "firstparty/APIs/chatAPI.h"
/**
 * @file globals.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Definitions of global variables used in the lobby.
 */

#include "utils/globals.h"
#include "utils/userTypes.h"

/** @brief Main window rectangle (set at init). */
Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
/** @brief Array of loaded fonts in increasing sizes. */
Font      fonts[_fontSizeCount] = {0};
/** @brief Primary font for UI text. */
Font      appFont = {0};

/** @brief Array of platforms in the lobby. */
Platform_st platforms[] = {
    // Sol principal
    {{-1000, 500, 2000, 1000}, GREEN, 0},

    // Murs
    {{-1000, 0, 500, 500}, BLACK, 0},
    {{500, 0, 500, 500}, BLACK, 0},

    // Platformes
    {{-350, 400, 100, 30}, BROWN, 0.5},
    {{250, 400, 100, 30}, BROWN, 0.5},
    {{-200, 300, 100, 30}, BROWN, 0.5},
    {{100, 300, 100, 30}, BROWN, 0.5},
    {{-50, 200, 100, 30}, BROWN, 0.5},
    {{-350, 150, 100, 30}, BROWN, 0.5},
    {{250, 150, 100, 30}, BROWN, 0.5},
};
/** @brief Number of platforms in the lobby. */
int platformCount = sizeof(platforms) / sizeof(platforms[0]);

/** @brief Array of available player textures. */
Texture2D playerTextures[2] = {0};
/** @brief Number of player textures currently loaded. */
int playerTextureCount = 0;

/** @brief Default source rectangle for player textures. */
Rectangle defaultPlayerTextureRect = {
    20,
    60,
    50,
    50
};

/** @brief Rectangle for the skin selection button. */
Rectangle skinButtonRect = {
    WINDOW_WIDTH - 70,
    WINDOW_HEIGHT / 2.0f - 25,
    50,
    50
};

/** @brief Trigger zone for the King For Four game. */
Rectangle kingForFourZone = {
    -350, 400 - 60, 100, 60
};

/** @brief Flag indicating if the texture selection menu is open. */
bool isTextureMenuOpen = false;

/** @brief Texture for the skin button logo. */
Texture2D logoSkinButton;
ChatState_St g_chatState = {0};
