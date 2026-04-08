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
#include "utils/userTypes.h"

LobbyGame_St game = {0};

float gameTime = 0.0f;

Platform_St platforms[] = {
    // --- SOL ---
    { .rect = {-X_LIMIT, GROUND_Y, X_LIMIT * 2, 1000}, .textureId = PLATFORM_TEXTURE_GRASS_ID},

    // --- BAS DU TRONC (Fini l'escalier droit, on zigzag) ---
    { .rect = { -100, GROUND_Y - 100, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre
    { .rect = {  100, GROUND_Y - 220, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Décalé droite
    { .rect = { -280, GROUND_Y - 340, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Décalé gauche

    // --- PREMIER GROS ÉCARTEMENT (Les grosses branches du bas) ---
    { .rect = {  -50, GROUND_Y - 450, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-droit
    { .rect = { -480, GROUND_Y - 440, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Extrême gauche (sur les feuilles)
    { .rect = {  280, GROUND_Y - 460, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Extrême droite (sur les feuilles)

    // --- MILIEU DE L'ARBRE (On remonte sur les côtés) ---
    { .rect = { -350, GROUND_Y - 560, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Gauche
    { .rect = {  150, GROUND_Y - 580, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Droite
    { .rect = { -150, GROUND_Y - 670, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Retour vers le centre

    // --- EXTÉRIEURS HAUTS (On exploite vraiment la largeur) ---
    { .rect = { -520, GROUND_Y - 720, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Tout au bout à gauche
    { .rect = {  350, GROUND_Y - 740, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Tout au bout à droite

    // --- ON GRIMPE VERS LA CANOPÉE ---
    { .rect = { -280, GROUND_Y - 840, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Gauche
    { .rect = {   80, GROUND_Y - 860, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Droite
    { .rect = { -100, GROUND_Y - 960, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Jonction centrale

    // --- LES BRANCHES SUPÉRIEURES ---
    { .rect = { -380, GROUND_Y - 1020, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Haut gauche
    { .rect = {  250, GROUND_Y - 1050, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Haut droite
    { .rect = { -220, GROUND_Y - 1140, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-gauche
    { .rect = {   50, GROUND_Y - 1160, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Centre-droit

    // --- LE SOMMET DU FEUILLAGE ---
    { .rect = { -320, GROUND_Y - 1260, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID},
    { .rect = {  180, GROUND_Y - 1280, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID},
    { .rect = {  -80, GROUND_Y - 1380, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}, // Juste sous la pointe

    // --- LA CIME ABSOLUE ---
    { .rect = { -100, GROUND_Y - 1500, 200, PLAT_H}, .textureId = PLATFORM_TEXTURE_WOODPLANK_ID}
};

u32 platformCount = sizeof(platforms) / sizeof(platforms[0]);

Rectangle skinButtonRect = { WINDOW_WIDTH - 70, WINDOW_HEIGHT / 2.0f - 25, 50, 50 };

Texture2D logoSkinButton = {0};

Texture2D platformTextures[__platformTextureCount] = {0};

Texture2D treeTexture;
Texture2D backgroundTexture;

GrassBlade_St grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

const Vector2 moonLightDir = {-0.6f, -0.8f};

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound sound_doubleJumpMeme;