/**
    @file utils/globals.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-02-23
    @brief Definitions of program-wide global variables.

    Contributors:
        - LeandreB8:
            - Provided the initial variables and harcoded platform
        - Fshimi-Hawlk:
            - Provided documentation
            - Moved some of the variables to dedicated struct.

    Contains shared state and constants used across the application:
        - Window rectangle and UI button placement
        - Font handles
        - Mini-game instance pointers (games[])
        - Lobby platform definitions (platforms[] + platformCount)
        - Default player sprite rectangle
        - Shared player texture array
        - Skin menu toggle and button texture

    The lobby level geometry (platforms) is hard-coded here.
    Mini-game registration happens via the games[] array.
    Skin menu visibility is controlled by isTextureMenuOpen.

    @see `utils/globals.h` for type definitions and extern declarations
*/

#include "utils/globals.h"

float gameTime = 0.0f;

Platform_St platforms[] = {
    // --- SOL ---
    { .rect = {-X_LIMIT, GROUND_Y, X_LIMIT * 2, 1000}, .type = PLATFORM_TYPE_GRASS},

    // --- BAS DU TRONC (Fini l'escalier droit, on zigzag) ---
    { .rect = { -100, GROUND_Y -  100, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Centre
    { .rect = {  100, GROUND_Y -  220, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Décalé droite
    { .rect = { -280, GROUND_Y -  340, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Décalé gauche

    // --- PREMIER GROS ÉCARTEMENT (Les grosses branches du bas) ---
    { .rect = {    0, GROUND_Y -  500, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Centre-droit

    // --- EXTÉRIEURS HAUTS (On exploite vraiment la largeur) ---
    { .rect = { -600, GROUND_Y -  650, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Tout au bout à gauche
    { .rect = { -150, GROUND_Y -  700, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Retour vers le centre
    { .rect = {  350, GROUND_Y -  850, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Tout au bout à droite

    // --- ON GRIMPE VERS LA CANOPÉE ---
    { .rect = { -920, GROUND_Y -  940, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Haut gauche
    { .rect = { -100, GROUND_Y -  960, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Jonction centrale

    // --- LES BRANCHES SUPÉRIEURES ---
    { .rect = { -650, GROUND_Y - 1180, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Haut gauche
    { .rect = {  250, GROUND_Y - 1050, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Haut droite
    { .rect = { -220, GROUND_Y - 1140, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}, // Centre-gauche
    { .rect = {  620, GROUND_Y - 1200, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK},

    // --- LE SOMMET DU FEUILLAGE ---
    { .rect = { -490, GROUND_Y - 1350, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK},
    { .rect = {  180, GROUND_Y - 1280, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK},

    // --- LA CIME ABSOLUE ---
    { .rect = { -100, GROUND_Y - 1480, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK},
    { .rect = {  340, GROUND_Y - 1600, 200, PLAT_H}, .type = PLATFORM_TYPE_WOODPLANK}
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

ParamsMenu_St paramsMenu = {0};

Rectangle skinButtonRect = {0};
Texture2D logoSkinButton = {0};

Texture2D platformTextures[__platformTypeCount] = {0};

Texture2D treeTexture;
Texture2D backgroundTexture;

GrassBlade_St grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

const Vector2 moonLightDir = {-0.6f, -0.8f};

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound sound_doubleJumpMeme;
