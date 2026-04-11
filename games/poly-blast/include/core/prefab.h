/**
    @file shape.h (core/game)
    @author Fshimi Hawlk
    @date 2026-02-25
    @brief TODO
*/

#ifndef CORE_PREFAB_H
#define CORE_PREFAB_H

#include "utils/userTypes.h"

/**
    @brief Checks if two prefabs have identical block layouts via offsets.

    Uses a small hashmap (36 bytes, for 6x6 grid) to mark positions from
    the first prefab, then verifies all positions from the second are marked.

    @note The largest offsets is have a 6 either in the x or y (and 1 in the other component), 
          hence the 6x6 hashmap.
    @note Assumes offsets are normalized.

    @param prefab1 First prefab.
    @param prefab2 Second prefab.
    @return true if offsets match, false otherwise.
*/
bool polyBlast_haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2);

/**
    @brief Checks if the prefab is within board bounds at a given position.

    @param prefab Pointer to the prefab.
    @param pos Position of the top-level corner of the prefab.
    @param board Pointer to the board.
    @return true if in bounds, false otherwise.
*/
bool polyBlast_isPrefabInBoundAt(const Prefab_St* const prefab, const s8Vector2 pos, const Board_St* const board);

/**
    @brief Sets the bounding box (width/height) for a prefab.

    @param prefab Pointer to the prefab.
*/
void polyBlast_setPrefabBoundingBox(Prefab_St* const prefab);

/**
    @brief Gets the center of the prefab's offsets.

    @param prefab The prefab.
    @return The offset center vector.
*/
f32Vector2 polyBlast_getOffsetCenter(const Prefab_St prefab);

/**
    @brief Adds a prefab and its rotated/mirrored variants to the bag.

    Generates variants only if orientations > 0 or canMirror is true, avoiding duplicates.
    Each variant is a separate Prefab_St entry in the bag.

    @param prefab The base prefab.
    @param prefabsBag Pointer to the prefab bag.
*/
void polyBlast_addPrefabAndVariants(Prefab_St prefab, PrefabBagVec_St* const prefabsBag);

/**
    @brief Rotates the prefab's offsets by 90° × rotateBy times (clockwise).

    Rotation is done in-place on prefab->offsets[].
    The function uses standard 2D rotation formulas:
      new_x =  old_y
      new_y = -old_x

    After rotation the bounding box (width/height) is **not** automatically updated —
    call setPrefabBoundingBox() afterwards if needed.

    @note Does **not** check for duplicate orientations — that's done during initPrefab().

    @param prefab    Prefab to modify (offsets are mutated)
    @param rotateBy  Number of 90° clockwise rotations (0 = no-op, negative = counterclockwise)
*/
void polyBlast_rotatePrefab(Prefab_St* const prefab, u8 rotateBy);

/**
    @brief Mirrors the prefab horizontally.

    @param prefab Pointer to the prefab.
*/
void polyBlast_mirrorPrefab(Prefab_St* const prefab);

/**
    @brief Prints debug information about a prefab (block count, dimensions, etc.).

    @param prefab The prefab to print.
*/
void polyBlast_printPrefabInfo(const Prefab_St prefab);

#endif // CORE_PREFAB_H