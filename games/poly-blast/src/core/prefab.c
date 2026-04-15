/**
    @file prefab.c
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-04-14
    @brief Prefab manipulation and variant generation logic implementation.
*/
#include "core/prefab.h"
#include "core/board.h"

/**
    @brief Compares if two prefabs have the same block offsets.

    @param[in]     prefab1      First prefab.
    @param[in]     prefab2      Second prefab.
    @return                     true if they have similar offsets, false otherwise.
*/
bool polyBlast_haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2) {
    bool hashmap[36] = {0};
    bool same = true;

    for (u8 i = 0; i < prefab1.blockCount; ++i) {
        u8 index = prefab1.offsets[i].x * 6 + prefab1.offsets[i].y;
        hashmap[index] = true;
    }

    for (u8 i = 0; i < prefab2.blockCount; ++i) {
        u8 index = prefab2.offsets[i].x * 6 + prefab2.offsets[i].y;
        same &= hashmap[index];
    }

    return same;
}

/**
    @brief Checks if a prefab would be within board bounds if placed at pos.

    @param[in]     prefab       Pointer to the prefab.
    @param[in]     pos          Target position.
    @param[in]     board        Pointer to the board.
    @return                     true if in bounds, false otherwise.
*/
bool polyBlast_isPrefabInBoundAt(const Prefab_St* const prefab, const s8Vector2 pos, const Board_St* const board) {
    return polyBlast_isInBound(pos, board)
        && (pos.x + prefab->width <= board->width)
        && (pos.y + prefab->height <= board->height);
}

/**
    @brief Finds the minimum and maximum coordinates among a prefab's block offsets.

    @param[in]     offsets      Array of block offsets.
    @param[in]     blockCount   Number of blocks.
    @param[out]    min          Pointer to store minimum coordinates.
    @param[out]    max          Pointer to store maximum coordinates.
*/
static void findPrefabMinMax(const s8Vector2 *const offsets, const u8 blockCount, s8Vector2* const min, s8Vector2* const max) {
    min->x = offsets[0].x;
    min->y = offsets[0].y;
    max->x = min->x;
    max->y = min->y;

    for (u32 i = 1; i < blockCount; i++) {
        if (offsets[i].x < min->x) min->x = offsets[i].x;
        if (offsets[i].y < min->y) min->y = offsets[i].y;
        if (offsets[i].x > max->x) max->x = offsets[i].x;
        if (offsets[i].y > max->y) max->y = offsets[i].y;
    }
}

/**
    @brief Calculates and sets the bounding box (width/height) of a prefab.

    @param[in,out] prefab       Pointer to the prefab.
*/
void polyBlast_setPrefabBoundingBox(Prefab_St* const prefab) {
    s8Vector2 min, max;
    findPrefabMinMax((const s8Vector2 *const) prefab->offsets, prefab->blockCount, &min, &max);

    // Update width and height
    prefab->width = (max.x - min.x + 1);
    prefab->height = (max.y - min.y + 1);
}

/**
    @brief Gets the center point of the prefab's bounding box.

    @param[in]     prefab       The prefab.
    @return                     The center position vector.
*/
f32Vector2 polyBlast_getOffsetCenter(const Prefab_St prefab) {
    if (prefab.blockCount == 0) return (f32Vector2) {0};
    return (f32Vector2) { .x = prefab.width / 2.0f, .y = prefab.height / 2.0f };
}

/**
    @brief Generates all rotations and mirrored variants of a prefab and adds them to a bag.

    @param[in]     prefab       Base prefab.
    @param[in,out] prefabsBag   Bag to add variants to.
*/
void polyBlast_addPrefabAndVariants(Prefab_St prefab, PrefabBagVec_St* const prefabsBag) {
    da_append(prefabsBag, prefab);

    for (u8 k = 1; k < prefab.orientations; ++k) {
        polyBlast_rotatePrefab(&prefab, 1);
        da_append(prefabsBag, prefab);
    }

    if (!prefab.canMirror) return;

    polyBlast_rotatePrefab(&prefab, 1);
    polyBlast_mirrorPrefab(&prefab);
    da_append(prefabsBag, prefab);

    for (u8 k = 1; k < prefab.orientations; ++k) {
        polyBlast_rotatePrefab(&prefab, 1);
        da_append(prefabsBag, prefab);
    }
}

/**
    @brief Rotates a prefab's block offsets clockwise.

    @param[in,out] prefab       Pointer to the prefab.
    @param[in]     rotateBy     Number of 90-degree rotations.
*/
void polyBlast_rotatePrefab(Prefab_St* const prefab, u8 rotateBy) {
    if (prefab == NULL || prefab->blockCount == 0) return;

    rotateBy %= 4;

    s8Vector2 newOffsets[MAX_SHAPE_SIZE];
    memcpy(newOffsets, prefab->offsets, prefab->blockCount * sizeof(*prefab->offsets));

    // Apply rotations
    for (u8 i = 0; i < prefab->blockCount; ++i) {
        // Clockwise rotation: (x, y) -> (y, -x)
        for (u8 r = 0; r < rotateBy; r++) {
            s8 x = newOffsets[i].x;
            s8 y = newOffsets[i].y;
            newOffsets[i].x = y;
            newOffsets[i].y = -x;
        }
    }

    s8Vector2 min, max;
    findPrefabMinMax(newOffsets, prefab->blockCount, &min, &max);

    // Shift offsets
    for (u8 i = 0; i < prefab->blockCount; i++) {
        newOffsets[i].x -= min.x;
        newOffsets[i].y -= min.y;
    }

    // Update width and height
    prefab->width = (max.x - min.x + 1);
    prefab->height = (max.y - min.y + 1);

    memcpy(prefab->offsets, newOffsets, prefab->blockCount * sizeof(*prefab->offsets));
}

/**
    @brief Mirrors a prefab's block offsets horizontally.

    @param[in,out] prefab       Pointer to the prefab.
*/
void polyBlast_mirrorPrefab(Prefab_St* const prefab) {
    if (prefab == NULL || prefab->blockCount == 0) return;

    s8Vector2 newOffsets[MAX_SHAPE_SIZE];
    memcpy(newOffsets, prefab->offsets, prefab->blockCount * sizeof(*prefab->offsets));

    for (u32 i = 0; i < prefab->blockCount; ++i) {
        newOffsets[i].x = -newOffsets[i].x;
        newOffsets[i].y = newOffsets[i].y;
    }

    s8Vector2 min, max;
    findPrefabMinMax(newOffsets, prefab->blockCount, &min, &max);

    // Shift offsets
    for (u8 i = 0; i < prefab->blockCount; i++) {
        newOffsets[i].x -= min.x;
        newOffsets[i].y -= min.y;
    }

    memcpy(prefab->offsets, newOffsets, prefab->blockCount * sizeof(*prefab->offsets));
}
