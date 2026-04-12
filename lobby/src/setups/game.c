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
static LobbyTerrain_St __fallbackTerrainContent[] = {
    { .rect = {-1000, 500, 2000, 1000}, .color = {0, 228, 48, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL, .velocity = {0,0} },
    { .rect = {-1000, 0, 500, 500},     .color = {0, 0, 0, 255},     .roundness = 0.0f, .type = TERRAIN_NORMAL, .velocity = {0,0} },
    { .rect = {500, 0, 500, 500},       .color = {0, 0, 0, 255},     .roundness = 0.0f, .type = TERRAIN_NORMAL, .velocity = {0,0} },
    { .rect = {-350, 400, 100, 30},     .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {250, 400, 100, 30},      .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {-200, 300, 100, 30},     .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {100, 300, 100, 30},      .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {-50, 200, 100, 30},      .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {-350, 150, 100, 30},     .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
    { .rect = {250, 150, 100, 30},      .color = {127, 106, 79, 255}, .roundness = 0.5f, .type = TERRAIN_WOOD,   .velocity = {0,0} },
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
