#include "global.h"
#include "config.h"

Platform_st platforms[] = {
    // --- SOL ---
    {{-X_LIMIT, GROUND_Y, X_LIMIT * 2, 1000}, PLATFORM_TEXTURE_GRASS_ID},

    // --- BAS DU TRONC (Fini l'escalier droit, on zigzag) ---
    {{ -100, GROUND_Y - 100, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre
    {{  100, GROUND_Y - 220, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Décalé droite
    {{ -280, GROUND_Y - 340, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Décalé gauche

    // --- PREMIER GROS ÉCARTEMENT (Les grosses branches du bas) ---
    {{  -50, GROUND_Y - 450, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-droit
    {{ -480, GROUND_Y - 440, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Extrême gauche (sur les feuilles)
    {{  280, GROUND_Y - 460, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Extrême droite (sur les feuilles)

    // --- MILIEU DE L'ARBRE (On remonte sur les côtés) ---
    {{ -350, GROUND_Y - 560, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Gauche
    {{  150, GROUND_Y - 580, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Droite
    {{ -150, GROUND_Y - 670, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Retour vers le centre

    // --- EXTÉRIEURS HAUTS (On exploite vraiment la largeur) ---
    {{ -520, GROUND_Y - 720, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Tout au bout à gauche
    {{  350, GROUND_Y - 740, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Tout au bout à droite

    // --- ON GRIMPE VERS LA CANOPÉE ---
    {{ -280, GROUND_Y - 840, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Gauche
    {{   80, GROUND_Y - 860, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Droite
    {{ -100, GROUND_Y - 960, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Jonction centrale

    // --- LES BRANCHES SUPÉRIEURES ---
    {{ -380, GROUND_Y - 1020, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Haut gauche
    {{  250, GROUND_Y - 1050, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Haut droite
    {{ -220, GROUND_Y - 1140, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-gauche
    {{   50, GROUND_Y - 1160, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-droit

    // --- LE SOMMET DU FEUILLAGE ---
    {{ -320, GROUND_Y - 1260, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID},
    {{  180, GROUND_Y - 1280, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID},
    {{  -80, GROUND_Y - 1380, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}, // Juste sous la pointe

    // --- LA CIME ABSOLUE ---
    {{ -100, GROUND_Y - 1500, 200, PLAT_H}, PLATFORM_TEXTURE_WOODPLANK_ID}
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
    WINDOW_HEIGHT / 2 - 25,
    50,
    50
};

bool isTextureMenuOpen = false;

Texture2D logoSkinButton;

Texture2D platformTextures[] = {
    [PLATFORM_TEXTURE_GRASS_ID]     = {0},
    [PLATFORM_TEXTURE_WOODPLANK_ID] = {0},
};
int platformTexturesCount;

Texture2D texTree;
Texture2D texBackground;

float gameTime = 0.0f;

GrassBlade_st grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;
