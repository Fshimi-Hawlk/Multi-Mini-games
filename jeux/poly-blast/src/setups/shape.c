/**
    @file shape.c (setups)
    @author Fshimi Hawlk
    @date 2026-01-07
    @date 2026-04-09
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
    polyBlast_setPrefabBoundingBox(prefab);

    Prefab_St prefabCmp = *prefab;
    Prefab_St prefabCmpMirror = *prefab;

    polyBlast_rotatePrefab(&prefabCmp, 1);

    polyBlast_mirrorPrefab(&prefabCmpMirror);
    prefab->canMirror = !polyBlast_haveSimilarOffsets(*prefab, prefabCmpMirror);

    while (!polyBlast_haveSimilarOffsets(*prefab, prefabCmp)) {
        prefab->orientations++;
        polyBlast_rotatePrefab(&prefabCmp, 1);
    }

    prefab->orientations += prefab->orientations > 0;

    if (!prefab->canMirror) return;

    bool foundDuplicate = false;
    for (u8 i = 0; i < prefab->orientations; ++i) {
        for (u8 j = 0; j < prefab->orientations; ++j) {
            if (polyBlast_haveSimilarOffsets(prefabCmp, prefabCmpMirror)) {
                foundDuplicate = true;
                break;
            }

            polyBlast_rotatePrefab(&prefabCmp, 1);
        }

        if (foundDuplicate) break;
        polyBlast_rotatePrefab(&prefabCmpMirror, 1);
    }

    if (foundDuplicate) {
        prefab->canMirror = false;
    }
}

void polyBlast_initPrefabsAndVariants(PrefabBagVec_St* const prefabsBag, GamePrefabVariant_Et variant) {
    u8 initCount = variant == GAME_PREFAB_VARIANT_DEFAULT
                 ? _prefabNameCount
                 : polyBlast_prefabCount;

    da_clear(prefabsBag);

    for (u32 i = 0; i < initCount; ++i) {
        Prefab_St prefab = polyBlast_prefabs[i];

        if (prefab.orientations > -1) {
            log_warn("Prefab %u hasn't setup correctly in the codebase.", i);
        } else {
            initPrefab(&prefab);
        }

        polyBlast_addPrefabAndVariants(prefab, prefabsBag);
    }

    for (u8 i = 1; i < MAX_SHAPE_SIZE; ++i) { // hence 0 is left to zero intentionally
        polyBlast_prefabsPerSizeOffsets[i] = prefabsBag->count - 1;
    }

    // it assumed that prefabBag is sorted by blockCount ascending
    // if it's init via `initPrefabsAndVariants`, then it is.
    for (u32 i = 1; i < prefabsBag->count; ++i) {
        if (prefabsBag->items[i - 1].blockCount != prefabsBag->items[i].blockCount) {
            polyBlast_prefabsPerSizeOffsets[(prefabsBag->items[i].blockCount - 1)] = i;
        }
    }

    // back propagation in case of gaps avoiding wrong max for unset offsets
    for (u8 i = MAX_SHAPE_SIZE - 2; i > 0; --i) {
        if (polyBlast_prefabsPerSizeOffsets[i] == prefabsBag->count - 1)
            polyBlast_prefabsPerSizeOffsets[i] = polyBlast_prefabsPerSizeOffsets[i + 1];
    }
}