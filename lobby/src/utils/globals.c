/**
 * @file globals.c
 * @author 
 * @date 
 * @brief Definitions of global variables.
 */

#include "utils/globals.h"
#include "utils/userTypes.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

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
int platformCount = sizeof(platforms) / sizeof(platforms[0]);

Texture2D playerTextures[2] = {0}; // à revoir pour une gestion dynamique
int playerTextureCount = 0;

Rectangle defaultPlayerTextureRect = {
    20,
    60,
    50,
    50
};

Rectangle skinButtonRect = {
    WINDOW_WIDTH - 70,
    WINDOW_HEIGHT / 2.0f - 25,
    50,
    50
};

bool isTextureMenuOpen = false;

Texture2D logoSkinButton;