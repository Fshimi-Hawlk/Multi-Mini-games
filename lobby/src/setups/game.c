/**
    @file setups/game.c
    @author Grok (assisted)
    @date 2026-03-30
    @brief Implementation of central game state management.
*/

#include "setups/game.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

static LobbyTerrain_St _generatedTerrainContent[] = {
    // ==================================================================
    // Normal
    // ==================================================================
    { .rect = {-1126.1f, -628.1f, 3889.2f, 299.3f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-148.0f, 276.7f, 2736.7f, 517.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {2074.3f, -429.7f, 687.8f, 711.2f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-355.2f, 96.9f, 106.0f, 28.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-489.2f, 210.9f, 104.0f, 37.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-259.2f, 276.9f, 165.0f, 29.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-1143.2f, -426.1f, 682.0f, 715.0f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {1795.6f, 65.7f, 114.3f, 21.4f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {1988.2f, -24.1f, 114.3f, 21.4f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {2001.3f, 146.3f, 114.3f, 21.4f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },

    // ==================================================================
    // Stone
    // ==================================================================
    { .rect = {520.5f, 250.0f, 1879.5f, 30.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-145.5f, 73.9f, 655.4f, 204.8f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {506.1f, 157.8f, 90.0f, 120.9f}, .color = {139, 69, 19, 255}, .roundness = 0.0f },
    { .rect = {-1373.2f, 14.9f, 911.0f, 332.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {482.3f, -118.5f, 1470.3f, 30.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {281.7f, -16.1f, 158.3f, 25.8f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-2687.3f, -345.2f, 1607.8f, 480.1f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3823.6f, -456.5f, 1236.0f, 460.2f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4899.7f, -451.5f, 1416.1f, 531.2f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5589.7f, -234.0f, 1594.1f, 353.7f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4959.6f, 63.7f, 556.0f, 232.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5111.6f, 381.8f, 316.0f, 410.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5003.6f, 759.8f, 288.0f, 604.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4859.6f, 1099.8f, 256.0f, 652.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4753.6f, 1535.8f, 1084.0f, 342.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4735.6f, 1373.8f, 208.0f, 230.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4639.6f, 1481.8f, 180.0f, 96.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3711.6f, 1607.8f, 1212.0f, 326.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-2549.6f, 1569.8f, 1234.0f, 254.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-1483.5f, 1429.8f, 642.0f, 366.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-923.5f, 1657.8f, 1294.0f, 198.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {164.5f, 1389.8f, 782.0f, 508.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {860.5f, 1203.8f, 532.0f, 460.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {1842.5f, 1323.8f, 138.0f, 632.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {432.5f, 2271.8f, 1546.0f, 300.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {1260.5f, 1897.8f, 206.0f, 142.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-718.8f, 1773.8f, 593.3f, 673.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-345.5f, 2240.5f, 476.7f, 296.7f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-278.8f, 2153.8f, 340.0f, 120.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {1921.2f, 987.1f, 443.3f, 636.7f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2007.9f, 780.5f, 400.0f, 283.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-6096.4f, 1649.7f, 405.0f, 3155.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5931.4f, 1334.7f, 1270.0f, 485.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5801.4f, 1614.7f, 450.0f, 530.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5906.4f, 3349.7f, 450.0f, 1205.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5896.3f, 4479.7f, 2125.0f, 945.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-5641.4f, 4149.7f, 560.0f, 515.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4081.3f, 4579.7f, 3060.0f, 1015.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-1321.3f, 4004.7f, 2605.0f, 1730.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {1228.7f, 2909.7f, 1665.0f, 2135.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2268.7f, 1429.7f, 560.0f, 285.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2723.7f, 1564.7f, 525.0f, 415.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {3063.7f, 1804.7f, 235.0f, 765.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2853.7f, 2354.7f, 575.0f, 555.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2523.7f, 2579.7f, 680.0f, 630.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2298.8f, 614.6f, 1480.0f, 830.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2803.8f, 1004.6f, 1265.0f, 810.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {3093.8f, 1439.6f, 1230.0f, 3155.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {2428.8f, 2789.7f, 980.0f, 2235.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {678.7f, 3419.7f, 1185.0f, 1825.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-2161.4f, 4189.7f, 1140.0f, 1635.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-6098.2f, -102.0f, 1206.0f, 1785.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4261.6f, 63.2f, 195.0f, 168.8f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4236.6f, 199.4f, 150.0f, 225.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4205.3f, 395.7f, 83.8f, 166.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-4185.3f, 510.7f, 48.8f, 131.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3105.8f, -279.4f, 195.0f, 168.8f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3080.8f, -143.2f, 150.0f, 225.0f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3049.6f, 53.1f, 83.8f, 166.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },
    { .rect = {-3029.6f, 168.1f, 48.8f, 131.3f}, .color = {160, 160, 160, 255}, .roundness = 0.0f },

    // ==================================================================
    // Ice
    // ==================================================================
    { .rect = {-1358.2f, 542.9f, 889.0f, 57.0f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-1948.3f, 385.9f, 401.0f, 32.0f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-2686.3f, 341.9f, 285.0f, 27.0f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-2220.3f, 729.9f, 377.0f, 515.3f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-3171.3f, 506.9f, 347.0f, 29.0f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-4233.3f, 759.1f, 1261.9f, 102.4f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-3731.6f, 360.1f, 272.3f, 61.5f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },
    { .rect = {-3512.5f, 619.2f, 272.2f, 24.4f}, .color = {180, 220, 255, 255}, .roundness = 0.0f },

    // ==================================================================
    // Bouncy
    // ==================================================================
    { .rect = {-1737.3f, 639.9f, 197.0f, 29.0f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-2254.3f, 511.9f, 127.0f, 36.0f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-2701.3f, 648.9f, 103.0f, 21.0f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-540.9f, 743.3f, 428.0f, 30.0f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-3646.0f, 732.2f, 136.9f, 30.8f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-2106.3f, 699.9f, 102.2f, 37.8f}, .color = {255, 100, 0, 255}, .roundness = 0.0f },
    { .rect = {-400.0f, 475.0f, 161.0f, 28.0f}, .color = {255, 100, 0, 255}, .roundness = 0.5f },

    // ==================================================================
    // Water
    // ==================================================================
    { .rect = {-5757.2f, 788.0f, 8945.8f, 3942.5f}, .color = {30, 100, 200, 180}, .roundness = 0.0f },
    { .rect = {-3949.7f, 250.9f, 189.2f, 309.2f}, .color = {30, 100, 200, 180}, .roundness = 0.0f },
    { .rect = {-3717.1f, 153.0f, 228.5f, 118.5f}, .color = {30, 100, 200, 180}, .roundness = 0.0f },
    { .rect = {-3403.7f, 220.7f, 222.3f, 99.2f}, .color = {30, 100, 200, 180}, .roundness = 0.0f },

    // ==================================================================
    // Portal
    // ==================================================================
    { .rect = {-4734.7f, 4193.6f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {488.9f, -248.9f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {1877.5f, -243.0f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {641.8f, -35.2f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {1818.6f, -46.3f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-25.0f, -200.0f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-1500.0f, 200.0f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-4034.0f, 142.2f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-4746.0f, 664.2f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-2813.3f, 1441.5f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-19.2f, 1983.5f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {2115.9f, 1748.5f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {912.6f, 3161.9f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-2739.6f, 3023.6f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
    { .rect = {-5439.7f, 2338.6f, 50.0f, 50.0f}, .color = {200, 0, 255, 180}, .roundness = 1.0f },
};

static void initTerrains(TerrainVec_St* const terrains) {
    // Initialize dynamic array with reasonable starting capacity
    da_reserve(terrains, 64);

    // Copy initial hardcoded terrains into dynamic array
    for (u32 i = 0; i < ARRAY_LEN(_generatedTerrainContent); ++i) {
        da_append(terrains, _generatedTerrainContent[i]);
    }

    log_info("Game initialized with %zu dynamic terrains", terrains->count);
}

Error_Et gameInit(void) {
    skinButtonRect.x = systemSettings.video.width - 70;
    skinButtonRect.y = systemSettings.video.height / 2.0f - 25;
    skinButtonRect.width = 50;
    skinButtonRect.height = 50;

    initTerrains(&terrains);
    return OK;
}