/**
    @file setups/game.c
    @author Grok (assisted)
    @date 2026-03-27
    @brief Implementation of central game state management.
*/

#include "setups/game.h"
#include "editor/io.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

#include "APIs/generalAPI.h"

// Dynamic terrain array (replaces static terrains[])
static LobbyTerrain_St _oldTerrainContent[] = {
    // ==================================================================
    // MAIN GROUND (spans entire lobby for basic navigation)
    // ==================================================================
    { {-1800.0f, 550.0f, 4200.0f, 150.0f}, {139, 69, 19, 255}, 0.0f, TERRAIN_NORMAL, {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // ==================================================================
    // LEFT SIDE: ISOLATED TEST LEVELS (one level per terrain type)
    // Bottom level = water as requested
    // ==================================================================
    // Level 1 (bottom) – Water test pool
    { {-1600.0f, 520.0f, 800.0f, 40.0f}, {30, 100, 200, 180}, 0.8f, TERRAIN_WATER,     {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 2 – Normal terrain test
    { {-1600.0f, 400.0f, 300.0f, 30.0f}, {139, 69, 19, 255}, 0.0f, TERRAIN_NORMAL,    {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 3 – Wood terrain test
    { {-1200.0f, 400.0f, 300.0f, 30.0f}, {139, 69, 19, 255}, 0.3f, TERRAIN_WOOD,      {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 4 – Stone terrain test
    { {-800.0f,  400.0f, 300.0f, 30.0f}, {160, 160, 160, 255}, 0.0f, TERRAIN_STONE,   {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 5 – Ice terrain test
    { {-400.0f,  400.0f, 300.0f, 30.0f}, {180, 220, 255, 255}, 0.2f, TERRAIN_ICE,     {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 6 – Bouncy terrain test
    { {-1600.0f, 250.0f, 300.0f, 30.0f}, {255, 100, 0, 255}, 0.4f, TERRAIN_BOUNCY,    {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Level 7 – Moving horizontal test
    { {-1200.0f, 250.0f, 300.0f, 30.0f}, {70, 130, 180, 255}, 0.0f, TERRAIN_MOVING_H, {4.0f, 0.0f}, 220.0f, {0.0f, 0.0f}, false, false },

    // Level 8 – Moving vertical test
    { {-800.0f,  200.0f, 300.0f, 30.0f}, {70, 130, 180, 255}, 0.0f, TERRAIN_MOVING_V, {0.0f, -3.5f}, 180.0f, {0.0f, 0.0f}, false, false },

    // Level 9 – Decorative test (non-colliding visual)
    { {-400.0f,  250.0f, 300.0f, 30.0f}, {34, 139, 34, 255}, 0.0f, TERRAIN_DECORATIVE,{0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // ==================================================================
    // CENTER CONNECTING PATH (links left test area to right side)
    // ==================================================================
    { {-100.0f, 400.0f, 600.0f, 30.0f}, {139, 69, 19, 255}, 0.3f, TERRAIN_WOOD, {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // ==================================================================
    // RIGHT SIDE: MINI-GAME INTERACTION ZONES + LARGE UNDERLYING PLATFORM
    // ==================================================================
    // Large solid platform directly under all interaction zones
    { {600.0f, 250.0f, 1800.0f, 30.0f}, {160, 160, 160, 255}, 0.0f, TERRAIN_STONE, {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, false, false },

    // Example portals (placed for future teleport testing)
    { {400.0f,  300.0f, 80.0f, 80.0f}, {200, 0, 255, 180}, 1.0f, TERRAIN_PORTAL, {0.0f, 0.0f}, 0.0f, {1200.0f, 100.0f}, true, false },
    { {1800.0f, 300.0f, 80.0f, 80.0f}, {200, 0, 255, 180}, 1.0f, TERRAIN_PORTAL, {0.0f, 0.0f}, 0.0f, {-500.0f, 400.0f}, false, false },
    { {2800.0f, 180.0f, 80.0f, 80.0f}, {200, 0, 255, 180}, 1.0f, TERRAIN_PORTAL, {0.0f, 0.0f}, 0.0f, {0.0f, 0.0f}, true, false }
};

Error_Et gameInit(void) {
    skinButtonRect.x = systemSettings.video.width - 70;
    skinButtonRect.y = systemSettings.video.height / 2.0f - 25;
    skinButtonRect.width = 50;
    skinButtonRect.height = 50;

    // Initialize dynamic array with reasonable starting capacity
    da_reserve(&terrains, 64);

    bool ok = editorLoadLevel(ASSET_PATH "levels/lobby.dat");
    if (!ok) {
        // Copy initial hardcoded terrains into dynamic array
        for (u32 i = 0; i < ARRAY_LEN(_oldTerrainContent); ++i) {
            da_append(&terrains, _oldTerrainContent[i]);
        }
    }


    log_info("Game initialized with %zu dynamic terrains", terrains.count);

    return OK;
}