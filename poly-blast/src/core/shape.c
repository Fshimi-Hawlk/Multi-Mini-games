/**
    @file shape.c
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Implementation of shape handling and manipulation.
*/

#include "core/game.h"
#include "core/board.h"
#include "core/shape.h"
#include "core/prefab.h"
#include "core/placement.h"

#include "utils/globals.h"

#include "utils/random.h"
#include "utils/container.h"
#include "utils/debug.h"

bool isShapeClicked(const Shape_St* const shape) {
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

bool isShapeInBound(const Shape_St* const shape, const Board_St* const board) {
    s8Vector2 shapeBoardPos = mapShapeToBoardPos(shape, board);

    return isPrefabInBoundAt(shape->prefab, shapeBoardPos, board);
}

/**
    @brief Gets the top-left corner position of the shape.

    @param shape Pointer to the active shape.
    @return The top-left position vector.
*/
static f32Vector2 getShapeTopLeftCorner(const Shape_St* const shape) {
    u8Vector2 shapeBaseBoundingBox = {shape->prefab->width, shape->prefab->height};
    f32Vector2 shapeBoundingbox = vec2Scale(shapeBaseBoundingBox, BLOCK_PX_SIZE / 2.0f, f32Vector2);
    return Vector2Subtract(shape->center, shapeBoundingbox);
}

f32Vector2 getIthBlockPosition(const Shape_St shape, const u8 i) {
    const f32Vector2 offsetCenter = getOffsetCenter(*shape.prefab);

    return (f32Vector2) {
        .x = shape.center.x - (f32) (offsetCenter.x - shape.prefab->offsets[i].x) * BLOCK_PX_SIZE,
        .y = shape.center.y - (f32) (offsetCenter.y - shape.prefab->offsets[i].y) * BLOCK_PX_SIZE
    };
}

s8Vector2 mapShapeToBoardPos(const Shape_St* const shape, const Board_St* const board) {
    f32Vector2 shapeBoardPos = Vector2Subtract(Vector2AddValue(getShapeTopLeftCorner(shape), BLOCK_PX_SIZE / 2.0f), board->pos);
    return vec2Scale(shapeBoardPos, 1.0f/BLOCK_PX_SIZE, s8Vector2);
}

void handleShape(GameState_St* const game, Shape_St* const shape) {
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
            releaseShape(shape, &game->board);

            if (shape->placed) {
                if (checkBoardForClearing(&game->board)) {
                    PlaySound(sound_lineBreak);
                    clearBoard(&game->board);
                }

                u8 currentStreakCount = game->scoring.streakCount;
                manageScoreAndStreak(&game->scoring, &game->board, shape->prefab->blockCount);

                if (currentStreakCount < game->scoring.streakCount) {
                    PlaySound(sound_combo);
                }
            }

            if (testGameOver(game->board, game->prefabManager.slots)) {
                PlaySound(sound_gameOver);
                game->gameOver = true;
            }
        }
    }
}

/**
    @brief Re-populates all size-grouped prefab index bags from the master prefabsBag.

    This function is called after consuming prefabs (typically via shuffleSlots() -> randomizeShape())
    to make sure every size group that can still offer shapes has its indices ready.

    Behavior:
      - Only refills a bag if it is currently empty (count == 0)
      - Uses precomputed `prefabsPerSizeOffsets[]` to know the start index and range
        of each size group inside the flat `prefabsBag.items` array
      - For the last size group (size == MAX_SHAPE_SIZE-1), the range goes until the end
        of prefabsBag
      - Copies consecutive indices (start + k) into the bag
      - Shuffles the bag contents so the order of delivery is randomized each refill

    @note Edge cases handled:
      - If a size group has no prefabs at all -> bag remains count==0 (skipped)
      - If a bag still has items -> left untouched (avoids unnecessary work and preserves
        any remaining shuffle order from previous refill)

    @note Performance:
      - O(total number of prefabs in all refilled groups) per call
      - Usually fast because only empty bags are processed
*/
static void refillShapeBags(PrefabManager_St* const manager) {
    for (u8 i = 0; i < MAX_SHAPE_SIZE; ++i) {
        PrefabIndexBagVec_St* bag = &manager->bags[i];

        // Skip refill if this size still has unused prefabs
        if (bag->count != 0) continue;

        u32 start = prefabsPerSizeOffsets[i];

        // Compute how many prefabs exist for this size
        bag->count = i == MAX_SHAPE_SIZE - 1
                   ? prefabsBag.count - start
                   : prefabsPerSizeOffsets[i + 1] - start;

        // no shape of that size in the prefabs;
        if (bag->count == 0) continue;

        // Fill with consecutive indices from the master bag
        for (u32 k = 0; k < bag->count; ++k) bag->items[k] = start + k;

        // Randomize delivery order for this size group
        da_shuffleXor(bag, rand);
    }
}

/**
    @brief Picks a non-empty prefab index bag using current size-based weights.

    Performs weighted random selection among shape sizes (1 to MAX_SHAPE_SIZE).
    The probability of choosing size k is roughly proportional to weights[k].

    Implementation detail:
      - Generates uniform random value in [0,1)
      - Walks cumulative sum until it exceeds the random value
      - If the selected bag happens to be empty (count == 0), the whole draw is repeated

    @note This is rejection sampling: it keeps trying until it finds a bag that still has prefabs left.
    @note In normal play this almost never loops more than once or twice.
    @note If **all** bags become empty at the same time -> infinite loop.

    @param manager   Game's prefabs manager
    @return          Pointer to one of the global `bags[]` entries that has count > 0
*/
static PrefabIndexBagVec_St* getRandomPrefabBag(PrefabManager_St* const manager) {
    u8 sizeIdx;

    do {
        f32 prob = randfloat();
        f32 weightedSum = 0.0f;
        for (sizeIdx = 0; sizeIdx < MAX_SHAPE_SIZE; ++sizeIdx) {
            weightedSum += manager->sizeWeights.runTimeWeights[sizeIdx];
            if (prob <= weightedSum) break;
        }
    } while (manager->bags[sizeIdx].count == 0);

    return &manager->bags[sizeIdx];
}

/**
    @brief Replaces the prefab and visual state of one shape slot with a random new one.

    Resets:
      - position to default slot location
      - color to uniform random color
      - placed and dragging flags

    Then:
      1. Asks getRandomPrefabBag() for a non-empty bag (weighted by size)
      2. Pops the last index from that bag (treats it as a LIFO stack)
      3. Assigns the corresponding prefab from prefabsBag

    @note Important side effect:
      Decrements `.count` in one of the global bags[] arrays.
      If that bag reaches 0, future calls may reject that size until refilled.

    @note Does **not** check whether the bag actually had items left — relies on
    getRandomPrefabBag() to never return an empty bag.

    @param shape     One of the three slots (game->slots[0..2]) — modified in-place
    @param manager   Game's prefabs manager
*/
static void randomizeShape(Shape_St* const shape, PrefabManager_St* manager) {
    shape->center = defaultPositions[shape->id];
    shape->colorIndex = rand() % _blockColorCount;
    shape->placed = false;

    PrefabIndexBagVec_St* bag = getRandomPrefabBag(manager);
    u32 prefab_idx = bag->items[--bag->count];
    shape->prefab = &prefabsBag.items[prefab_idx];
}

void shuffleSlots(PrefabManager_St* const manager) {
    memset(manager->slots, 0, sizeof(manager->slots));
    for (u8 i = 0; i < 3; ++i) {
        manager->slots[i].id = i;
        randomizeShape(&manager->slots[i], manager);
    }

    refillShapeBags(manager);
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

void displayShape(const Shape_St* const shape) {
    if (shape->prefab == NULL || shape->prefab->blockCount == 0) {
        printf("(empty)\n");
        return;
    }

    Color c = blockColors[shape->colorIndex];

    bool grid[9][9] = {false};   /* MAX_SHAPE_SIZE == 9, bbox never bigger*/
    for (u8 i = 0; i < shape->prefab->blockCount; ++i) {
        grid[shape->prefab->offsets[i].y][shape->prefab->offsets[i].x] = true;
    }

    for (u8 y = 0; y < shape->prefab->height; ++y) {
        for (u8 x = 0; x < shape->prefab->width; ++x) {
            if (grid[y][x]) {
                printf("\033[38;2;%u;%u;%um██\033[0m", c.r, c.g, c.b);
            } else {
                printf("  ");
            }
        }
        nl
    }
}

void printSlotsGraphically(const ShapeSlots_t slots) {
    bool presenceGrid[9][(9 + 1) * 3] = {0};   /* MAX_SHAPE_SIZE == 9: bbox never bigger, +1 of padding*/
    Color colorGrid[9][(9 + 1) * 3] = {0};   /* MAX_SHAPE_SIZE == 9: bbox never bigger, +1 of padding*/
    
    u8 maxHeight = 0;
    u8 xOffset = 0;

    for (u8 shapeId = 0; shapeId < 3; ++shapeId) {
        Shape_St shape = slots[shapeId];

        for (u8 i = 0; i < shape.prefab->blockCount; ++i) {
            u8 x = shape.prefab->offsets[i].x + xOffset;
            u8 y = shape.prefab->offsets[i].y;
            presenceGrid[y][x] = true;
            colorGrid[y][x] = blockColors[shape.colorIndex];
        }

        if (shape.prefab->height > maxHeight) maxHeight = shape.prefab->height;
        xOffset += shape.prefab->width + 1;
    }

    for (u8 y = 0; y < maxHeight; ++y) {
        for (u8 x = 0; x < 30; ++x) {
            if (presenceGrid[y][x]) {
                printf("\033[38;2;%u;%u;%um██\033[0m", colorGrid[y][x].r, colorGrid[y][x].g, colorGrid[y][x].b);
            } else {
                printf("  ");
            }
        }
        nl
    }
}