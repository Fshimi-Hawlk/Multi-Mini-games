/**
    @file prefab.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Static blueprint (prefab) logic and variant generation.
*/
#ifndef CORE_PREFAB_H
#define CORE_PREFAB_H

#include "utils/userTypes.h"

/**
    @brief Checks if two prefabs have identical block layouts via offsets.

    Uses a small hashmap (36 bytes, for 6x6 grid) to mark positions from
    the first prefab, then verifies all positions from the second are marked.

    @param[in]     prefab1      First prefab to compare.
    @param[in]     prefab2      Second prefab to compare.
    @return                     true if offsets match, false otherwise.
*/
bool polyBlast_haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2);

/**
    @brief Checks if the prefab is within board bounds at a given position.

    @param[in]     prefab       Pointer to the prefab.
    @param[in]     pos          Position of the top-level corner of the prefab.
    @param[in]     board        Pointer to the board.
    @return                     true if in bounds, false otherwise.
*/
bool polyBlast_isPrefabInBoundAt(const Prefab_St* const prefab, const s8Vector2 pos, const Board_St* const board);

/**
    @brief Sets the bounding box (width/height) for a prefab based on its offsets.

    @param[in,out] prefab       Pointer to the prefab to update.
*/
void polyBlast_setPrefabBoundingBox(Prefab_St* const prefab);

/**
    @brief Gets the center of the prefab's offsets.

    @param[in]     prefab       The prefab.
    @return                     The offset center vector.
*/
f32Vector2 polyBlast_getOffsetCenter(const Prefab_St prefab);

/**
    @brief Adds a prefab and its rotated/mirrored variants to the bag.

    Generates variants only if orientations > 0 or canMirror is true, avoiding duplicates.
    Each variant is a separate Prefab_St entry in the bag.

    @param[in]     prefab       The base prefab.
    @param[in,out] prefabsBag   Pointer to the prefab bag to populate.
*/
void polyBlast_addPrefabAndVariants(Prefab_St prefab, PrefabBagVec_St* const prefabsBag);

/**
    @brief Rotates the prefab's offsets by 90° × rotateBy times (clockwise).

    Rotation is done in-place on prefab->offsets[].

    @param[in,out] prefab       Prefab to modify (offsets are mutated).
    @param[in]     rotateBy     Number of 90° clockwise rotations.
*/
void polyBlast_rotatePrefab(Prefab_St* const prefab, u8 rotateBy);

/**
    @brief Mirrors the prefab horizontally.

    @param[in,out] prefab       Pointer to the prefab to modify.
*/
void polyBlast_mirrorPrefab(Prefab_St* const prefab);

/**
    @brief Prints debug information about a prefab to the console.

    @param[in]     prefab       The prefab to print.
*/
void polyBlast_printPrefabInfo(const Prefab_St prefab);

#endif // CORE_PREFAB_H