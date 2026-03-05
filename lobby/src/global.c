#include "global.h"
#include "constant.h"
#include "config.h"

// --- CONFIGURATION DU LOBBY ---
#define PLAT_W        120
#define PLAT_H         30
#define DOOR_H        200  // Hauteur de l'ouverture pour les jeux
#define SCREEN_H      800  // Distance entre chaque étage de jeu
#define STEP_Y         70  // Écart vertical entre deux plateformes (facile à monter)
#define WALL_W        500
#define GROUND_Y      500

Platform_st platforms[] = {
    // --- SOL ---
    {{-1000, GROUND_Y, 2000, 1000}, GREEN, 0},

    // =========================================================
    // ÉTAGE 1 (Y = 100 environ)
    // =========================================================
    // Murs avec ouverture de DOOR_H
    {{-1000, 200, WALL_W, 300}, BLACK, 0}, 
    {{500,   200, WALL_W, 300}, BLACK, 0},   
    {{-1000, 200 - DOOR_H - 600, WALL_W, 600}, BLACK, 0}, 
    {{500,   200 - DOOR_H - 600, WALL_W, 600}, BLACK, 0},   

    // Plateformes d'ascension (Calculées sur STEP_Y)
    {{-100, GROUND_Y - (1*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ 150, GROUND_Y - (2*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{-200, GROUND_Y - (3*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{  50, GROUND_Y - (4*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ 280, GROUND_Y - (5*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{-320, GROUND_Y - (6*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ -50, GROUND_Y - (7*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},

    // =========================================================
    // ÉTAGE 2 (Y = -700 environ)
    // =========================================================
    // Murs avec ouverture
    {{-1000, -600, WALL_W, 600}, BLACK, 0}, 
    {{500,   -600, WALL_W, 600}, BLACK, 0},
    {{-1000, -600 - DOOR_H - 600, WALL_W, 600}, BLACK, 0}, 
    {{500,   -600 - DOOR_H - 600, WALL_W, 600}, BLACK, 0},

    // Suite de l'ascension
    {{ 100, GROUND_Y - (8*STEP_Y),  PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ 320, GROUND_Y - (9*STEP_Y),  PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ -80, GROUND_Y - (10*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{-300, GROUND_Y - (11*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{  50, GROUND_Y - (12*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ 250, GROUND_Y - (13*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{-120, GROUND_Y - (14*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{ 180, GROUND_Y - (15*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{-250, GROUND_Y - (16*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5},
    {{  20, GROUND_Y - (17*STEP_Y), PLAT_W, PLAT_H}, BROWN, 0.5}
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