/**
    @file shape.c (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Prefab initialization and variant computation.
*/

#include "setups/shape.h"

#include "core/prefab.h"

#include "utils/globals.h"

/**
    @brief Initializes a single prefab, computing orientations and mirroring.

    Rotates and compares offsets to find unique orientations. Checks for mirror duplicates.
    Updates prefab->orientations and ->canMirror accordingly.

    @param prefab Pointer to the prefab to initialize.
*/
static void initPrefab(Prefab_St* const prefab) {
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

    bool foundDuplicate = false;
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

void polyBlast_initPrefabsAndVariants(PrefabBagVec_St* const prefabsBag, GamePrefabVariant_Et variant) {
    u8 initCount = variant == GAME_PREFAB_VARIANT_DEFAULT
                 ? _prefabNameCount
                 : prefabCount;

    da_clear(prefabsBag);

    for (u32 i = 0; i < initCount; ++i) {
        Prefab_St prefab = prefabs[i];

        if (prefab.orientations > -1) {
            log_warn("Prefab %u hasn't setup correctly in the codebase.", i);
        } else {
            initPrefab(&prefab);
        }

        addPrefabAndVariants(prefab, prefabsBag);
    }

    for (u8 i = 1; i < MAX_SHAPE_SIZE; ++i) { // hence 0 is left to zero intentionally
        prefabsPerSizeOffsets[i] = prefabsBag->count - 1;
    }

    // it assumed that prefabBag is sorted by blockCount ascending
    // if it's init via `initPrefabsAndVariants`, then it is.
    for (u32 i = 1; i < prefabsBag->count; ++i) {
        if (prefabsBag->items[i - 1].blockCount != prefabsBag->items[i].blockCount) {
            prefabsPerSizeOffsets[(prefabsBag->items[i].blockCount - 1)] = i;
        }
    }

    // back propagation in case of gaps avoiding wrong max for unset offsets
    for (u8 i = MAX_SHAPE_SIZE - 2; i > 0; --i) {
        if (prefabsPerSizeOffsets[i] == prefabsBag->count - 1)
            prefabsPerSizeOffsets[i] = prefabsPerSizeOffsets[i + 1];
    }
}