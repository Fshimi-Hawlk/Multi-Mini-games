/**
 * @file shape.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Implementation of shape handling and manipulation.
 */

#include "core/game/game.h"
#include "core/game/board.h"
#include "core/game/shape.h"

#include "utils/common.h"
#include "utils/globals.h"
#include "utils/userTypes.h"
#include "utils/utils.h"

bool haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2) {
    bool hashmap[36] = {0};
    bool same = true;

    for (u8 i = 0; i < prefab1.blockCount; ++i) {
        u8 index = prefab1.offsets[i].x * 6 + prefab1.offsets[i].y;
        hashmap[index] = true;
    }

    for (u8 i = 0; i < prefab1.blockCount; ++i) {
        u8 index = prefab2.offsets[i].x * 6 + prefab2.offsets[i].y;
        same &= hashmap[index];
    }

    return same;
}

bool isShapeClicked(const ActivePrefab_St* const shape) {
    f32Vector2 mousePos = GetMousePosition();

    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        f32Vector2 blockPos = getIthBlockPosition(*shape, j);
        Rectangle blockRec = {
            .x = blockPos.x, .y = blockPos.y,
            .width = BLOCK_PX_SIZE, .height = BLOCK_PX_SIZE
        };

        if (CheckCollisionPointRec(mousePos, blockRec)) return true;
    }

    return false;
}

bool isShapeInBound(const ActivePrefab_St* const shape, const Board_St* const board) {
    s8Vector2 shapeBoardPos = mapShapeToBoardPos(shape, board);

    return isInBound(shapeBoardPos, board)
        && (shapeBoardPos.x + shape->prefab->width <= board->width)
        && (shapeBoardPos.y + shape->prefab->height <= board->height);
}

bool isShapePlaceable(const ActivePrefab_St *const shape, const s8Vector2 pos, const Board_St* const board) {
    bool canBePlaced = isShapeInBound(shape, board);
    if (!canBePlaced) return false;

    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        u8Vector2 blockPos = {
            .x = pos.x + shape->prefab->offsets[j].x,
            .y = pos.y + shape->prefab->offsets[j].y
        };

        canBePlaced &= board->blocks[blockPos.y][blockPos.x].hitsLeft == 0;
    }

    return canBePlaced;
}

/**
 * @brief Finds the minimum and maximum coordinates among a prefab's block offsets.
 *
 * Iterates through the provided offsets and computes the bounding box min/max values.
 * Used during rotation/mirroring to normalize offsets.
 *
 * @param offsets       Array of block offsets.
 * @param blockCount    Number of blocks in the prefab.
 * @param outMin        Output: minimum x/y coordinates.
 * @param outMax        Output: maximum x/y coordinates.
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

void setPrefabBoundingBox(Prefab_St* const prefab) {
    s8Vector2 min, max;
    findPrefabMinMax((const s8Vector2 *const) prefab->offsets, prefab->blockCount, &min, &max);

    // Update width and height
    prefab->width = (max.x - min.x + 1);
    prefab->height = (max.y - min.y + 1);
}

f32Vector2 getShapeTopLeftCorner(const ActivePrefab_St* const shape) {
    u8Vector2 shapeBaseBoundingBox = {shape->prefab->width, shape->prefab->height};
    f32Vector2 shapeBoundingbox = vec2Scale(shapeBaseBoundingBox, BLOCK_PX_SIZE / 2.0f, f32Vector2);
    return Vector2Subtract(shape->center, shapeBoundingbox);
}

f32Vector2 getOffsetCenter(const Prefab_St prefab) {
    if (prefab.blockCount == 0) return (f32Vector2) {0};
    return (f32Vector2) { .x = prefab.width / 2.0f, .y = prefab.height / 2.0f };
}

f32Vector2 getIthBlockPosition(const ActivePrefab_St shape, const u8 i) {
    const f32Vector2 offsetCenter = getOffsetCenter(*shape.prefab);

    return (f32Vector2) {
        .x = shape.center.x - (f32) (offsetCenter.x - shape.prefab->offsets[i].x) * BLOCK_PX_SIZE,
        .y = shape.center.y - (f32) (offsetCenter.y - shape.prefab->offsets[i].y) * BLOCK_PX_SIZE
    };
}

s8Vector2 mapShapeToBoardPos(const ActivePrefab_St* const shape, const Board_St* const board) {
    f32Vector2 shapeBoardPos = Vector2Subtract(Vector2AddValue(getShapeTopLeftCorner(shape), BLOCK_PX_SIZE / 2.0f), board->pos);
    return vec2Scale(shapeBoardPos, 1.0f/BLOCK_PX_SIZE, s8Vector2);
}

void addPrefabAndVariants(Prefab_St prefab, PrefabBagVec_St* const prefabsBag) {
    da_append(prefabsBag, prefab);

    for (u8 k = 1; k < prefab.orientations; ++k) {
        rotatePrefab(&prefab, 1);
        da_append(prefabsBag, prefab);
    }

    if (!prefab.canMirror) return;

    rotatePrefab(&prefab, 1);
    mirrorPrefab(&prefab);
    da_append(prefabsBag, prefab);

    for (u8 k = 1; k < prefab.orientations; ++k) {
        rotatePrefab(&prefab, 1);
        da_append(prefabsBag, prefab);
    }
}

void handleShape(ActivePrefab_St* const shape) {
    if (shape->placed) return;

    f32Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragging) {
        if (isShapeClicked(shape)) {
            shape->dragging = true;
            dragging = true;
            mouseDeltaFromShapeCenter = Vector2Subtract(shape->center, mousePos);
        }
    } else if (shape->dragging) {
        shape->center = Vector2Add(mousePos, mouseDeltaFromShapeCenter);
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (shape->dragging) {
            releaseShape(shape, &game.board);

            if (shape->placed) {
                manageScore(&game, shape->prefab->blockCount);
            }
        }
    }
}

/**
 * @brief Re-populates all size-grouped prefab index bags from the master prefabsBag.
 *
 * This function is called after consuming prefabs (typically via shuffleSlots() -> randomizeShape())
 * to make sure every size group that can still offer shapes has its indices ready.
 *
 * Behavior:
 *   - Only refills a bag if it is currently empty (count == 0)
 *   - Uses precomputed `prefabsPerSizeOffsets[]` to know the start index and range
 *     of each size group inside the flat `prefabsBag.items` array
 *   - For the last size group (size == MAX_SHAPE_SIZE-1), the range goes until the end
 *     of prefabsBag
 *   - Copies consecutive indices (start + k) into the bag
 *   - Shuffles the bag contents so the order of delivery is randomized each refill
 *
 * @note Edge cases handled:
 *   - If a size group has no prefabs at all -> bag remains count==0 (skipped)
 *   - If a bag still has items -> left untouched (avoids unnecessary work and preserves
 *     any remaining shuffle order from previous refill)
 *
 * @note Performance:
 *   - O(total number of prefabs in all refilled groups) per call
 *   - Usually fast because only empty bags are processed
 */
static void refillShapeBags(PrefabManager_St* const manager) {
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        PrefabIndexBagVec_St* bag = &manager->bags[i];

        // Skip refill if this size still has unused prefabs
        if (bag->count != 0) continue;

        u32 start = prefabsPerSizeOffsets[i];

        // Compute how many prefabs exist for this size
        bag->count = i == MAX_SHAPE_SIZE - 1
                   ? manager->prefabsBag.count - start
                   : prefabsPerSizeOffsets[i + 1] - start;

        // no shape of that size in the prefabs;
        if (bag->count == 0) continue;

        // Fill with consecutive indices from the master bag
        for (u32 k = 0; k < bag->count; ++k) bag->items[k] = start + k;

        // Randomize delivery order for this size group
        da_shuffle(bag);
    }
}

/**
 * @brief Picks a non-empty prefab index bag using current size-based weights.
 *
 * Performs weighted random selection among shape sizes (1 to MAX_SHAPE_SIZE).
 * The probability of choosing size k is roughly proportional to weights[k].
 *
 * Implementation detail:
 *   - Generates uniform random value in [0,1)
 *   - Walks cumulative sum until it exceeds the random value
 *   - If the selected bag happens to be empty (count == 0), the whole draw is repeated
 *
 * @note This is rejection sampling: it keeps trying until it finds a bag that still has prefabs left.
 * @note In normal play this almost never loops more than once or twice.
 * @note If **all** bags become empty at the same time -> infinite loop.
 *
 * @param manager   Game's prefabs manager
 * @return          Pointer to one of the global `bags[]` entries that has count > 0
 */
static PrefabIndexBagVec_St* getRandomPrefabBag(PrefabManager_St* const manager) {
    u8 sizeIdx;

    do {
#ifndef _USE_DEFAULT_RAND
        f32 prob = prng_randf();
#else
        f32 prob = randfloat();
#endif
        f32 weightedSum = 0.0f;
        for (sizeIdx = 0; sizeIdx < manager->bags->count; ++sizeIdx) {
            weightedSum += manager->sizeWeights.runTimeWeights[sizeIdx];
            if (prob <= weightedSum) break;
        }
    } while (manager->bags[sizeIdx].count == 0);

    return &manager->bags[sizeIdx];
}

/**
 * @brief Replaces the prefab and visual state of one shape slot with a random new one.
 *
 * Resets:
 *   - position to default slot location
 *   - color to uniform random color
 *   - placed and dragging flags
 *
 * Then:
 *   1. Asks getRandomPrefabBag() for a non-empty bag (weighted by size)
 *   2. Pops the last index from that bag (treats it as a LIFO stack)
 *   3. Assigns the corresponding prefab from prefabsBag
 *
 * @note Important side effect:
 *   Decrements `.count` in one of the global bags[] arrays.
 *   If that bag reaches 0, future calls may reject that size until refilled.
 *
 * @note Does **not** check whether the bag actually had items left — relies on
 * getRandomPrefabBag() to never return an empty bag.
 *
 * @param shape     One of the three slots (game->slots[0..2]) — modified in-place
 * @param manager   Game's prefabs manager
 */
void randomizeShape(ActivePrefab_St* const shape, PrefabManager_St* manager) {
    shape->center = defaultPositions[shape->id];
    shape->colorIndex = RAND_FUNC() % _blockColorCount;
    shape->placed = false;

    PrefabIndexBagVec_St* bag = getRandomPrefabBag(manager);
    u32 prefab_idx = bag->items[--bag->count];
    shape->prefab = &manager->prefabsBag.items[prefab_idx];
}

/**
 * @brief Algorithm for automatic shape placement (WIP).
 *
 * Tries to find optimal positions for shapes to maximize clears and score.
 * Currently incomplete; simulates placements on algoGame state.
 *
 * @param game Pointer to the game state.
 */
void placingAlgo(const GameState_St* const game) {
    GameState_St algoGame = {0};
    memcpy(&algoGame, game, sizeof(algoGame));

    DA(u8Vector2) cellIndices = {0};
    for (u8 row = 0; row < algoGame.board.height; ++row) {
        for (u8 col = 0; col < algoGame.board.width; ++col) {
            if (algoGame.board.blocks[row][col].hitsLeft != 0) continue;
            u8Vector2 pos = {col, row};
            da_append(&cellIndices, pos);
        }
    }

    for (u32 i = 0; i < 3; ++i) {
        ActivePrefab_St* shape = &algoGame.prefabManager.slots[i];
        randomizeShape(shape, &algoGame.prefabManager);

        da_foreach(u8Vector2, cellIndex, &cellIndices) {
            if (!isShapePlaceable(shape, castTo(s8Vector2) cellIndex, &game->board)) continue;
            placeShape(shape, *cellIndex, &algoGame.board);
            break;
        }

        // removes every cell that was filled
        for (u32 i = 0; i < cellIndices.count; ++i) {
            u8Vector2 cellIndex = cellIndices.items[i];
            if (algoGame.board.blocks[cellIndex.y][cellIndex.x].hitsLeft == 0) continue;
            da_remove_unordered(&cellIndices, i);
        }
    }
}

void shuffleSlots(PrefabManager_St* const manager) {
    for (u8 i = 0; i < 3; ++i) {
        manager->slots[i].id = i;
        randomizeShape(&manager->slots[i], manager);
    }

    refillShapeBags(manager);
}

void placeShape(const ActivePrefab_St* const shape, const u8Vector2 pos, Board_St* const board) {
    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        u8Vector2 blockPos = {
            .x = pos.x + shape->prefab->offsets[j].x,
            .y = pos.y + shape->prefab->offsets[j].y
        };

        board->blocks[blockPos.y][blockPos.x].hitsLeft = 1;
        board->blocks[blockPos.y][blockPos.x].colorIndex = shape->colorIndex;
    }
}

void rotatePrefab(Prefab_St* const prefab, u8 rotateBy) {
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

void mirrorPrefab(Prefab_St* const prefab) {
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

void releaseShapeAt(ActivePrefab_St *const shape, s8Vector2 pos, Board_St *const board) {
    shape->dragging = false;
    dragging = false;

    if (isShapePlaceable(shape, pos, board)) {
        placeShape(shape, castTo(u8Vector2) pos, board);
        shape->placed = true;
    } else {
        shape->center = defaultPositions[shape->id];
    }
}

void releaseShape(ActivePrefab_St* const shape, Board_St* const board) {
    releaseShapeAt(shape, mapShapeToBoardPos(shape, board), board);
}

void printPrefabInfo(const Prefab_St prefab) {
    f32Vector2 offsetCenter = getOffsetCenter(prefab);
    printf("| bc: %u | w: %u, h: %u | c: (%.1f, %.1f) | o: %d | canMirror: %s, offsets: ",
        prefab.blockCount, prefab.width, prefab.height, offsetCenter.x, offsetCenter.y, prefab.orientations, boolStr(prefab.canMirror)
    );

    for (u8 i = 0; i < prefab.blockCount; ++i) {
        printf("(%u| %d, %d) ", i, prefab.offsets[i].x, prefab.offsets[i].y);
    }
    nl
}