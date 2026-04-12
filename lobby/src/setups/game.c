/**
    @file setups/game.c
    @author Fshimi-Hawlk
    @author i-Charlys
    @date 2026-03-30
    @date 2026-04-10
    @brief Implementation of central game state management and level loading.
*/

#include "editor/io.h"

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"
#include "sharedUtils/container.h"

static void initGrass(void) {
    Rectangle floor = terrains.items[0].rect;
    grassCount = 0;

    float stepX = 3.0f;
    float stepY = 5.0f;

    for (float y = floor.y; y < floor.y + 500.0f; y += stepY) {
        for (float x = -X_LIMIT - 500; x < X_LIMIT + 500; x += stepX) {
            if (grassCount >= MAX_GRASS_BLADES) break;

            float offX = (float)(rand() % 15) - 7.5f;
            float offY = (float)(rand() % 10);

            float baseHeight;
            int type = rand() % 10;
            if (type < 7) baseHeight = 5.0f + (rand() % 10);
            else if (type < 9) baseHeight = 15.0f + (rand() % 15);
            else baseHeight = 2.0f + (rand() % 4);

            float depth = (y - floor.y) / floor.height;
            float colorVar = (float)(rand() % 35);

            grassBlades[grassCount] = (GrassBlade_St){
                .position = { x + offX, y + offY },
                .height = baseHeight,
                .angle = 0.0f,
                .velocity = 0.0f,
                .color = (Color){
                    clamp(35 + colorVar - (depth * 15), 10, 255),
                    clamp(90 + colorVar - (depth * 70), 20, 180),
                    clamp(25 - (depth * 10), 5, 255),
                    255
                }
            };
            grassCount++;
        }
    }
}

/**
    @brief Fallback terrain content - Restored from 22-03 branch 
*/
static LobbyTerrain_St __fallbackTerrainContent[] =  {
    // --- SOL ---
    { .rect = {-X_LIMIT, GROUND_Y, X_LIMIT * 2, 1000}, .kind = TERRAIN_KIND_GRASS},

    // --- BAS DU TRONC (Fini l'escalier droit, on zigzag) ---
    { .rect = { -100, GROUND_Y -  100, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Centre
    { .rect = {  100, GROUND_Y -  220, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Décalé droite
    { .rect = { -280, GROUND_Y -  340, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Décalé gauche

    // --- PREMIER GROS ÉCARTEMENT (Les grosses branches du bas) ---
    { .rect = {    0, GROUND_Y -  500, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Centre-droit

    // --- EXTÉRIEURS HAUTS (On exploite vraiment la largeur) ---
    { .rect = { -600, GROUND_Y -  650, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Tout au bout à gauche
    { .rect = { -150, GROUND_Y -  700, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Retour vers le centre
    { .rect = {  350, GROUND_Y -  850, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Tout au bout à droite

    // --- ON GRIMPE VERS LA CANOPÉE ---
    { .rect = { -920, GROUND_Y -  940, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Haut gauche
    { .rect = { -100, GROUND_Y -  960, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Jonction centrale

    // --- LES BRANCHES SUPÉRIEURES ---
    { .rect = { -650, GROUND_Y - 1180, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Haut gauche
    { .rect = {  250, GROUND_Y - 1050, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Haut droite
    { .rect = { -220, GROUND_Y - 1140, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}, // Centre-gauche
    { .rect = {  620, GROUND_Y - 1200, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK},

    // --- LE SOMMET DU FEUILLAGE ---
    { .rect = { -490, GROUND_Y - 1350, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK},
    { .rect = {  180, GROUND_Y - 1280, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK},
    { .rect = { -100, GROUND_Y - 1480, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK},
    { .rect = {  340, GROUND_Y - 1600, 200, PLAT_H}, .kind = TERRAIN_KIND_WOOD_PLANK}
};

Error_Et lobby_gameInit(void) {
    skinButtonRect.x = GetScreenWidth() - 70;
    skinButtonRect.y = GetScreenHeight() / 2.0f - 25;
    skinButtonRect.width = 50;
    skinButtonRect.height = 50;

    // Initialize dynamic array with reasonable starting capacity
    da_reserve(&terrains, 64);

    // Try loading the default level
    bool ok = editorLoadLevel(ASSET_PATH "levels/lobby.dat");
    if (!ok) {
        log_warn("Failed to load lobby.dat, using fallback terrain");
        // Copy initial hardcoded terrains into dynamic array
        for (u32 i = 0; i < ARRAY_LEN(__fallbackTerrainContent); ++i) {
            da_append(&terrains, __fallbackTerrainContent[i]);
        }
    }

    log_info("Game initialized with %zu dynamic terrains", terrains.count);

    initGrass();

    return OK;
}
