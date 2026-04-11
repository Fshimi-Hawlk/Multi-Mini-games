/**
    @file utils/globals.c
    @author LeandreB8
    @date 2026-01-12
    @date 2026-04-10
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

// ────────────────────────────────────────────────
// General Globals
// ────────────────────────────────────────────────

LobbyGame_St game = {0};

Font lobby_fonts[__fontSizeCount] = {0};

f32 gameTime = 0.0f;

PlayerProgress_St g_progress = {0};
Chat_St gameChat = {0};

// ────────────────────────────────────────────────
// Platforms / Terrains
// ────────────────────────────────────────────────

TerrainVec_St terrains = {0};

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

Texture2D platformTextures[__platformTypeCount] = {0};

GameInteractionZone_St gameZones[__miniGameIdCount] = {
    [MINI_GAME_ID_TETRIS] = {
        .hitbox = {
            .x      = -800,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Tetris",
        .color = {0, 120, 220, 200},
    },

    [MINI_GAME_ID_SOLITAIRE] = {
        .hitbox = {
            .x      = -575,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Solitaire",
        .color = {0, 160, 80,  200},
    },

    [MINI_GAME_ID_SUIKA] = {
        .hitbox = {
            .x      = -350,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Suika",
        .color = {220, 80, 0,  200},
    },

    [MINI_GAME_ID_BOWLING] = {
        .hitbox = {
            .x      = -125,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Bowling",
        .color = {140, 0, 200, 200},
    },

    [MINI_GAME_ID_GOLF] = {
        .hitbox = {
            .x      = 100,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Golf 3D",
        .color = { 20, 160,  50, 200},
    },

    [MINI_GAME_ID_SNAKE] = {
        .hitbox = {
            .x      = 325,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "Snake",
        .color = {  0, 200,  80, 200},
    },

    [MINI_GAME_ID_POLY_BLAST] = {
        .hitbox = {
            .x      = 550,
            .y      = 425,
            .width  = 75,
            .height = 75
        },
        .name = "BlockBlast",
        .color = { 60,  60, 200, 200},
    },

    [MINI_GAME_ID_KFF] = {
        .hitbox = { 
            .x      = -350.0f, 
            .y      =  340.0f, 
            .width  =  100.0f, 
            .height =  60.0f 
        },
        .name = "King For Four"
    },

    [MINI_GAME_ID_CHESS] = {
        .hitbox = { 
            .x      =  250.0f, 
            .y      =  340.0f, 
            .width  =  100.0f, 
            .height =  60.0f 
        },
        .name = "Chess"
    },

    [MINI_GAME_ID_CUBE] = {
        .hitbox = { 
            .x      = -200.0f, 
            .y      =  240.0f, 
            .width  =  100.0f, 
            .height =  60.0f 
        },
        .name = "Rubik Cube"
    },

    [MINI_GAME_ID_BINGO] = {
        .hitbox = { 
            .x      =  100.0f, 
            .y      =  240.0f, 
            .width  =  100.0f, 
            .height =  60.0f 
        },
        .name = "Bingo"
    },

    [MINI_GAME_ID_EDITOR] = {
        .hitbox = { 
            .x      =  -50.0f, 
            .y      =  140.0f, 
            .width  =  100.0f, 
            .height =  60.0f 
        },
        .name = "Level Editor"
    },

    // Hide others for now
    [MINI_GAME_ID_BATTLESHIP] = {
        .hitbox = {0},
        .name = "Battleship" 
    },

    [MINI_GAME_ID_CONNECT_4]  = {
        .hitbox = {0},
        .name = "Connect 4" 
    },

    [MINI_GAME_ID_MINIGOLF]   = {
        .hitbox = {0},
        .name = "Mini Golf" 
    },

    [MINI_GAME_ID_MORPION]    = {
        .hitbox = {0},
        .name = "Morpion" 
    },

    [MINI_GAME_ID_OTHELLO]    = {
        .hitbox = {0},
        .name = "Othello" 
    },
};

// ────────────────────────────────────────────────
// Skin stuff
// ────────────────────────────────────────────────

Rectangle skinButtonRect = { WINDOW_WIDTH - 70, WINDOW_HEIGHT / 2.0f - 25, 50, 50 };

Texture2D logoSkinButton = {0};

// ────────────────────────────────────────────────
// Parameters Menu
// ────────────────────────────────────────────────

ParamsMenu_St paramsMenu;

// ────────────────────────────────────────────────
// Textures / Ambiant
// ────────────────────────────────────────────────

Texture2D treeTexture;
Texture2D backgroundTexture;

GrassBlade_St grassBlades[MAX_GRASS_BLADES];
int grassCount = 0;

const Vector2 moonLightDir = {-0.6f, -0.8f};

// ────────────────────────────────────────────────
// Sounds
// ────────────────────────────────────────────────

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound sound_doubleJumpMeme;

// ────────────────────────────────────────────────
// Physics Debug Panel
// ────────────────────────────────────────────────

bool showPhysicsDebugPanel = false;
f32 panelScrollY = 0.0f;
