#include "utils/globals.h"
#include "core/game/shape.h"
#include "setups/shape.h"

void initPrefabsAndVariants(Prefab_DA_St* const prefabsBag) {
    u8 initCount = game.prefabVariant == GAME_PREFAB_VARIANT_DEFAULT
                 ? _prefabNameCount : prefabCount;

    for (u32 i = 0; i < initCount; ++i) {
        Prefab_St prefab = prefabs[i];
        
        if (prefab.orientations > -1) {
            log_warn("Prefab %u hasn't setup correctly in the codebase.", i);
        } else {
            initPrefab(&prefab);
        }

        addPrefabAndVariants(prefab, prefabsBag);
    }
}

void initPrefab(Prefab_St* const prefab) {
    prefab->orientations = 0;
    setPrefabBoundingBox(prefab);

    Prefab_St prefabCmp = *prefab;
    Prefab_St prefabCmpMirror = *prefab;

    rotatePrefab(&prefabCmp, 1);

    mirrorPrefab(&prefabCmpMirror);
    prefab->canMirror = !haveSimilarOffsets(*prefab, prefabCmpMirror);
    
    while (!haveSimilarOffsets(*prefab, prefabCmp)) {
        prefab->orientations++;
        rotatePrefab(&prefabCmp, 1);
    }
    
    prefab->orientations += prefab->orientations > 0;

    if (!prefab->canMirror) return;

    bool8 foundDuplicate = false;
    for (u8 i = 0; i < prefab->orientations; ++i) {
        for (u8 j = 0; j < prefab->orientations; ++j) {
            if (haveSimilarOffsets(prefabCmp, prefabCmpMirror)) {
                foundDuplicate = true;
                break;
            }

            rotatePrefab(&prefabCmp, 1);
        }

        if (foundDuplicate) break;
        rotatePrefab(&prefabCmpMirror, 1);
    }
    
    if (foundDuplicate) {
        prefab->canMirror = false;
    }
}
