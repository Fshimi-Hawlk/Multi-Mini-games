#include <assert.h>

#include "utils/userTypes.h"

extern const Prefab_St prefabs[];
extern const u32 prefabCount;
const Prefab_St prefabs[1] = {{0}};  // Minimal mock
const u32 prefabCount = 1;

extern GameState_St game;
GameState_St game = {0};

#include "setups/shape.h"
#include "setups/shape.c"

s32 main(void) {
    printf("Running tests for setups/shape...\n");

    // Test initPrefabsAndVariants
    {
        PrefabBagVec_St bag = {0};
        game.prefabVariant = GAME_PREFAB_VARIANT_DEFAULT;  // Assume enum from userTypes

        initPrefabsAndVariants(&bag);
        assert(bag.count > 0 && "Bag should be initialized with prefabs");

        // Free bag if needed
    }

    // Test initPrefab
    {
        Prefab_St prefab = {0};
        prefab.orientations = -1;  // As per code

        initPrefab(&prefab);
        assert(prefab.orientations >= 0 && "Prefab orientations initialized");
    }

    printf("All tests passed for setups/shape!\n");
    return 0;
}