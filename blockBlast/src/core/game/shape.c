#include "core/game/game.h"
#include "core/game/board.h"
#include "core/game/shape.h"

#include "utils/globals.h"
#include "utils/utils.h"

bool8 haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2) {
    bool8 hashmap[36] = {0};
    bool8 same = true;

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

bool8 isShapeClicked(const ActivePrefab_St* const shape) {
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

bool8 isShapeInBound(const ActivePrefab_St* const shape) {
    s8Vector2 shapeBoardPos = mapShapeToBoardPos(shape);

    return isInBound(shapeBoardPos) && (shapeBoardPos.x + shape->prefab->width <= game.board.width) && (shapeBoardPos.y + shape->prefab->height <= game.board.height);
}

bool8 isShapePlaceable(const ActivePrefab_St* const shape) {
    s8Vector2 shapeBoardPos = mapShapeToBoardPos(shape);

    bool8 canBePlaced = isShapeInBound(shape);
    if (canBePlaced) {
        for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
            u8Vector2 blockPos = {
                .x = shapeBoardPos.x + shape->prefab->offsets[j].x,
                .y = shapeBoardPos.y + shape->prefab->offsets[j].y
            };

            canBePlaced &= game.board.blocks[blockPos.y][blockPos.x].hitsLeft == 0;
        }
    }

    return canBePlaced;
}

// Find min and max coordinates
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

s8Vector2 mapShapeToBoardPos(const ActivePrefab_St* const shape) {
    f32Vector2 shapeBoardPos = Vector2Subtract(Vector2AddValue(getShapeTopLeftCorner(shape), BLOCK_PX_SIZE / 2.0f), game.board.pos);
    return vec2Scale(shapeBoardPos, 1.0f/BLOCK_PX_SIZE, s8Vector2);
}

void addPrefabAndVariants(Prefab_St prefab, PrefabBag_St* const prefabsBag) {
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
                manageScore(&game, shape->prefab);
            }
        }
    }
}

void shuffleSlots(GameState_St* const game) {
    // the default weights
    f32 sizeWeights[MAX_BLOCK_PER_SHAPE] = {
        0.05f, 0.20f, 0.25f, 0.25f, 0.10f, 0.05f, 0.00f, 0.00f, 0.10f
    };

    for (u8 i = 0; i < 3; ++i) {
        ActivePrefab_St* const shape = &game->slots[i];
        shape->center = defaultPositions[i];
        shape->colorIndex = prng_rand() % _blockColorCount;
        shape->placed = false;
        shape->id = i;

        u8 sizeIdx;

        do {
            f32 prob = prng_randf();
            f32 weightedSum = 0.0f;
            for (sizeIdx = 0; sizeIdx < MAX_BLOCK_PER_SHAPE; ++sizeIdx) {
                weightedSum += sizeWeights[sizeIdx];
                if (prob <= weightedSum) break;
            }
        } while (bags[sizeIdx].count == 0);
    
        PrefabIndexBag_St* bag = &bags[sizeIdx];
        u32 prefab_idx = bag->items[--bag->count];
        shape->prefab = &prefabsBag.items[prefab_idx];
    }

    // refill if needed
    for (u8 i = 0; i < MAX_BLOCK_PER_SHAPE; ++i) {
        PrefabIndexBag_St* bag = &bags[i];
        if (bag->count == 0) {
            u32 start = prefabsPerSizeOffsets[i];
            bag->count = (i == MAX_BLOCK_PER_SHAPE - 1 ? prefabsBag.count - start : prefabsPerSizeOffsets[i + 1] - start);
            if (bag->count == 0) continue; // no shape of that size in the prefabs;
            
            for (u32 k = 0; k < bag->count; ++k) bag->items[k] = start + k;
            da_shuffle(bag);
        }
    }
}

void placeShape(const ActivePrefab_St* const shape, Board_St* const board) {
    s8Vector2 shapeBoardPos = mapShapeToBoardPos(shape);

    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        u8Vector2 blockPos = {
            .x = shapeBoardPos.x + shape->prefab->offsets[j].x,
            .y = shapeBoardPos.y + shape->prefab->offsets[j].y
        };
        board->blocks[blockPos.y][blockPos.x].hitsLeft = 1;
        board->blocks[blockPos.y][blockPos.x].colorIndex = shape->colorIndex;
    }
}

void rotatePrefab(Prefab_St* const prefab, u8 rotateBy) {
    if (prefab == NULL || prefab->blockCount == 0) return;

    rotateBy %= 4;

    s8Vector2 newOffsets[MAX_BLOCK_PER_SHAPE];
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

    s8Vector2 newOffsets[MAX_BLOCK_PER_SHAPE];
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

void releaseShape(ActivePrefab_St* const shape, Board_St* const board) {
    shape->dragging = false;
    dragging = false;

        if (isShapePlaceable(shape)) {
            placeShape(shape, board);
            shape->placed = true;
        }
    }
}

void printPrefabInfo(const Prefab_St prefab) {
    for (u8 i = 0; i < prefab.blockCount; ++i) {
        printf("(%d, %d) ", prefab.offsets[i].x, prefab.offsets[i].y);
    }

    f32Vector2 offsetCenter = getOffsetCenter(prefab);
    printf("| bc: %u | w: %u, h: %u | c: (%.1f, %.1f) | o: %d | canMirror: %s\n", 
        prefab.blockCount, prefab.width, prefab.height, offsetCenter.x, offsetCenter.y, prefab.orientations, boolStr(prefab.canMirror)
    );
}