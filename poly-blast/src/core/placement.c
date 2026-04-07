#include "core/placement.h"
#include "core/prefab.h"
#include "core/shape.h"
#include "core/board.h"
#include "core/game.h"

#include "utils/globals.h"

#include "utils/random.h"
#include "utils/container.h"

bool isShapePlaceable(const Shape_St *const shape, const s8Vector2 pos, const Board_St* const board) {
    bool canBePlaced = isPrefabInBoundAt(shape->prefab, pos, board);
    // log_debug("is prefab in bound at (" vec2siStr "): %s",vec2Fmt(pos), boolStr(canBePlaced));
    if (!canBePlaced) return false; // early exit to avoid unnecessary checks

    for (u8 i = 0; i < shape->prefab->blockCount; ++i) {
        u8Vector2 blockPos = {
            .x = pos.x + shape->prefab->offsets[i].x,
            .y = pos.y + shape->prefab->offsets[i].y
        };

        canBePlaced &= board->blocks[blockPos.y][blockPos.x].hitsLeft == 0;
    }

    return canBePlaced;
}

void placeShape(const Shape_St* const shape, const u8Vector2 pos, Board_St* const board) {
    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        u8Vector2 blockPos = {
            .x = pos.x + shape->prefab->offsets[j].x,
            .y = pos.y + shape->prefab->offsets[j].y
        };

        board->blocks[blockPos.y][blockPos.x].hitsLeft = 1;
        board->blocks[blockPos.y][blockPos.x].colorIndex = shape->colorIndex;
    }

    updateBoardClearing(board);
}

bool getRandomValidPosition(const Board_St* const board, const Shape_St* const shape, u8Vector2* const outPosition) {
    bool success = false;

    Arena* prev = contextArena;
    contextArena = &tempArena;
    Arena_Mark save = arena_snapshot(contextArena); {
        AnchorVec_St candidates = getAnchorCandidates(board, shape);
        
        if (candidates.count > 0) {
            da_shuffleT(u8Vector2, &candidates, rand);
            *outPosition = candidates.items[0];
            success = true;
        }
    } arena_rewind(contextArena, save);

    contextArena = prev;

    return success;
}

static bool tryGreedyPlaceShape(Board_St* const board, const Shape_St* const shape, ScoringState_St* const scoring) {
    u8Vector2 pos;
    if (!getRandomValidPosition(board, shape, &pos)) return false;

    placeShape(shape, pos, board);

    if (checkBoardForClearing(board)) {
        clearBoard(board);
    }

    if (scoring != NULL) {
        manageScoreAndStreak(scoring, board, shape->prefab->blockCount);
    }

    return true;
}

/**
    @brief Attempts to place the three slots onto a board copy in the exact
           order defined by `order` in a iterative way.

    For each slot:
      - Scans `emptyCells` (in the order it was passed) for the first position
        where `isShapePlaceable` succeeds.
      - Places the shape, immediately runs `checkBoardForClearing` + `clearBoard`.
      - If `scoring` is non-NULL, calls `manageScoreAndStreak` after each placement.

    Stops at the first shape that cannot be placed anywhere.

    @param[in,out] board        Board copy mutated by placements and clears.
    @param[in]     emptyCells   List of candidate anchor positions.
    @param[in]     slots        The three active prefabs to place.
    @param[in]     order        Permutation of {0,1,2} defining placement order.
    @param[in,out] scoring      Optional scoring state updated after each
                                successful placement and clear. Pass `NULL`
                                when only feasibility matters.

    @return     `true` if all three shapes were placed,
                `false` otherwise.
*/
static bool greedyPlaceAll(Board_St* const board, const ShapeSlots_t slots, const u8 order[3], ScoringState_St* const scoring) {
    for (u8 i = 0; i < 3; ++i) {
        const Shape_St* shape = &slots[order[i]];
        if (shape->placed) continue;
        if (!tryGreedyPlaceShape(board, shape, scoring)) return false;
    }
    
    return true;
}

bool canPlaceAll(Board_St* board, const ShapeSlots_t slots, const u8 order[3], u8 idx) {
    if (idx == 3) return true;

    const Shape_St* shape = &slots[order[idx]];
    if (shape->placed) return canPlaceAll(board, slots, order, idx+1);

    AnchorVec_St anchors = getAnchorCandidates(board, shape);

    // try every valid possible origin in shuffle order for more fairness of odd
    da_foreach(u8Vector2, anchor, &anchors) {
        Board_St simBoard = *board; // Avoid the need to backtrack when the attempt fails
        
        placeShape(shape, *anchor, &simBoard);
        if (checkBoardForClearing(&simBoard)) clearBoard(&simBoard);

        if (canPlaceAll(&simBoard, slots, order, idx+1)) return true;
    }

    return false;
}

void placementSimulation(GameState_St* const game) {
    f32 bestAttemptScore = 0;
    const Prefab_St* selectedPrefabs[3] = {0};
    bool success = false;

    // Set up `contextArena` to safely reset it, using `arena_reset`,
    // deallocate everything that was allocated using `context_alloc`
    contextArena = &tempArena;

    GameState_St algoGame = *game;

    // Need specialized function because of `bags`,
    // because the vectors contain allocated array `items`
    // that can't be simply copied with a single memcpy of the whole bag
    algoGame.prefabManager = deepcopyPrefabManager(&game->prefabManager);

    static const u8 permutations[6][3] = {
        {0, 1, 2}, {0, 2, 1},
        {1, 0, 2}, {1, 2, 0},
        {2, 0, 1}, {2, 1, 0}
    };

    for (u8 attempt = 0; attempt < 100; ++attempt) {
        shuffleSlots(&algoGame.prefabManager);

        for (u8 p = 0; p < 6; ++p) {
            ScoringState_St scoring = {
                .streakCount = algoGame.scoring.streakCount,
                .streakGrace = algoGame.scoring.streakGrace,
                .score = 0
            };

            Board_St attemptBoard = algoGame.board; // only swallow copy is needed

            if (greedyPlaceAll(&attemptBoard, algoGame.prefabManager.slots, permutations[p], &scoring)) {
                success = true;
                f32 recordedScore = scoring.score + scoring.streakCount * 1000;
                // log_debug("Recorded score: %f", recordedScore);
                
                if (bestAttemptScore < recordedScore) {
                    bestAttemptScore = recordedScore;

                    for (u8 i = 0; i < 3; ++i) {
                        selectedPrefabs[i] = algoGame.prefabManager.slots[permutations[p][i]].prefab;
                    }
                }
            }
        }

        if (!success) {
            if (attempt > 0 && attempt % 25 == 0) {
                adjustSizeWeights(game, 0);
            }
        } 
    }

    arena_reset(&tempArena);
    contextArena = &globalArena;

    if (success) {
        shuffleArrayT(const Prefab_St *, selectedPrefabs, 3, rand);
        for (u8 i = 0; i < 3; ++i) {
            game->prefabManager.slots[i].prefab = selectedPrefabs[i];
        }
    }
}


void releaseShapeAt(Shape_St *const shape, s8Vector2 pos, Board_St *const board) {
    shape->dragging = false;
    dragging = false;

    if (isShapeInBound(shape, board) && isShapePlaceable(shape, pos, board)) {
        previousGameState = mainGameState; // copy pre-release state as previous state
        placeShape(shape, castTo(u8Vector2) &pos, board);
        shape->placed = true;
        PlaySound(sound_shapePlacement);
    } else {
        shape->center = defaultPositions[shape->id];
    }
}

void releaseShape(Shape_St* const shape, Board_St* const board) {
    releaseShapeAt(shape, mapShapeToBoardPos(shape, board), board);
}