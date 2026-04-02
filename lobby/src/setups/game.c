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

/** @brief Fallback terrain content if loading fails */
static LobbyTerrain_St _fallbackTerrainContent[] = {
    { .rect = {-1126.1f, -628.1f, 3889.2f, 299.3f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {-148.0f, 276.7f, 2736.7f, 517.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {2074.3f, -429.7f, 687.8f, 711.2f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {-355.2f, 96.9f, 106.0f, 28.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {-489.2f, 210.9f, 104.0f, 37.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {-259.2f, 276.9f, 165.0f, 29.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    { .rect = {-1143.2f, -426.1f, 682.0f, 715.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f, .type = TERRAIN_NORMAL },
    
    // Water test pool
    { .rect = {-1600.0f, 520.0f, 800.0f, 40.0f}, .color = {30, 100, 200, 180}, .roundness = 0.8f, .type = TERRAIN_WATER },
    
    // Stone platform for games
    { .rect = {600.0f, 250.0f, 1800.0f, 30.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f, .type = TERRAIN_STONE }
};

Error_Et gameInit(void) {
    skinButtonRect.x = systemSettings.video.width - 70;
    skinButtonRect.y = systemSettings.video.height / 2.0f - 25;
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
