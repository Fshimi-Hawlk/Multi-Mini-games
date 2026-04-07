/**
    @file setups/game.c
    @author Grok (assisted)
    @author i-Charlys
    @date 2026-03-30
    @brief Implementation of central game state management and level loading.
*/

#include "setups/game.h"
#include "editor/io.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "systemSettings.h"
#include "APIs/generalAPI.h"

/** @brief Fallback terrain content - Restored from 22-03 branch */
static LobbyTerrain_St _fallbackTerrainContent[] = {
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

Error_Et gameInit(void) {
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
        for (u32 i = 0; i < ARRAY_LEN(_fallbackTerrainContent); ++i) {
            da_append(&terrains, _fallbackTerrainContent[i]);
        }
    }

    log_info("Game initialized with %zu dynamic terrains", terrains.count);

    return OK;
}
