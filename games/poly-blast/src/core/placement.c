/**
    @file placement.c
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Shape placement and simulation logic implementation.
*/
#include "core/placement.h"
#include "core/prefab.h"
#include "core/shape.h"
#include "core/board.h"
#include "core/game.h"

#include "utils/globals.h"

#include "sharedUtils/random.h"
#include "sharedUtils/container.h"

/**
    @brief Checks if a shape can be placed at a specific board position.

    @param[in]     shape        Pointer to the shape.
    @param[in]     pos          Board position.
    @param[in]     board        Pointer to the board.
    @return                     true if placeable, false otherwise.
*/
bool polyBlast_isShapePlaceable(const Shape_St *const shape, const s8Vector2 pos, const Board_St* const board) {
    bool canBePlaced = polyBlast_isPrefabInBoundAt(shape->prefab, pos, board);
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

/**
    @brief Places a shape onto the board at the given position.

    @param[in]     shape        Pointer to the shape.
    @param[in]     pos          Target board coordinates.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_placeShape(const Shape_St* const shape, const u8Vector2 pos, Board_St* const board) {
    for (u8 j = 0; j < shape->prefab->blockCount; ++j) {
        u8Vector2 blockPos = {
            .x = pos.x + shape->prefab->offsets[j].x,
            .y = pos.y + shape->prefab->offsets[j].y
        };

        board->blocks[blockPos.y][blockPos.x].hitsLeft = 1;
        board->blocks[blockPos.y][blockPos.x].colorIndex = shape->colorIndex;
    }

    polyBlast_updateBoardClearing(board);
}

/**
    @brief Gets a random valid position for a shape on the board.

    @param[in]     board        Pointer to the board.
    @param[in]     shape        Pointer to the shape.
    @param[out]    outPosition  The selected valid position.
    @return                     true if a position was found, false otherwise.
*/
bool polyBlast_getRandomValidPosition(const Board_St* const board, const Shape_St* const shape, u8Vector2* const outPosition) {
    bool success = false;

    Arena* prev = contextArena;
    contextArena = &tempArena;
    Arena_Mark save = arena_snapshot(contextArena); {
        AnchorVec_St candidates = polyBlast_getAnchorCandidates(board, shape);
        
        if (candidates.count > 0) {
            da_shuffleT(u8Vector2, &candidates, rand);
            *outPosition = candidates.items[0];
            success = true;
        }
    } arena_rewind(contextArena, save);

    contextArena = prev;

    return success;
}

/**
    @brief Attempts to place a shape greedily on the board.

    @param[in,out] board        Pointer to the board.
    @param[in]     shape        Pointer to the shape.
    @param[in,out] scoring      Optional scoring state to update.
    @return                     true if successfully placed, false otherwise.
*/
static bool tryGreedyPlaceShape(Board_St* const board, const Shape_St* const shape, ScoringState_St* const scoring) {
    u8Vector2 pos;
    if (!polyBlast_getRandomValidPosition(board, shape, &pos)) return false;

    polyBlast_placeShape(shape, pos, board);

    if (polyBlast_checkBoardForClearing(board)) {
        polyBlast_clearBoard(board);
    }

    if (scoring != NULL) {
        polyBlast_manageScoreAndStreak(scoring, board, shape->prefab->blockCount);
    }

    return true;
}

/**
    @brief Attempts to place the three slots onto a board copy greedily.

    @param[in,out] board        Board copy mutated by placements and clears.
    @param[in]     slots        The three active prefab slots.
    @param[in]     order        Permutation of {0,1,2} defining placement order.
    @param[in,out] scoring      Optional scoring state updated after each placement.
    @return                     true if all three shapes were placed, false otherwise.
*/
static bool greedyPlaceAll(Board_St* const board, const ShapeSlots_t slots, const u8 order[3], ScoringState_St* const scoring) {
    for (u8 i = 0; i < 3; ++i) {
        const Shape_St* shape = &slots[order[i]];
        if (shape->placed) continue;
        if (!tryGreedyPlaceShape(board, shape, scoring)) return false;
    }
    
    return true;
}

/**
    @brief Recursively checks if all shapes in slots can be placed in a specific order.

    @param[in,out] board        Pointer to the board (simulated state).
    @param[in]     slots        The three active prefab slots.
    @param[in]     order        Array of indices representing placement order.
    @param[in]     idx          Current index in the order array.
    @return                     true if all remaining shapes can be placed, false otherwise.
*/
bool polyBlast_canPlaceAll(Board_St* board, const ShapeSlots_t slots, const u8 order[3], u8 idx) {
    if (idx == 3) return true;

    const Shape_St* shape = &slots[order[idx]];
    if (shape->placed) return polyBlast_canPlaceAll(board, slots, order, idx+1);

    AnchorVec_St anchors = polyBlast_getAnchorCandidates(board, shape);

    // try every valid possible origin in shuffle order for more fairness of odd
    da_foreach(u8Vector2, anchor, &anchors) {
        Board_St simBoard = *board; // Avoid the need to backtrack when the attempt fails
        
        polyBlast_placeShape(shape, *anchor, &simBoard);
        if (polyBlast_checkBoardForClearing(&simBoard)) polyBlast_clearBoard(&simBoard);

        if (polyBlast_canPlaceAll(&simBoard, slots, order, idx+1)) return true;
    }

    return false;
}

/**
    @brief Runs a brute-force simulation to pick the "best" set of three prefabs.

    @param[in,out] game         Pointer to the current game state.
*/
void polyBlast_placementSimulation(GameState_St* const game) {
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
    algoGame.prefabManager = polyBlast_deepcopyPrefabManager(&game->prefabManager);

    static const u8 permutations[6][3] = {
        {0, 1, 2}, {0, 2, 1},
        {1, 0, 2}, {1, 2, 0},
        {2, 0, 1}, {2, 1, 0}
    };

    for (u8 attempt = 0; attempt < 100; ++attempt) {
        polyBlast_shuffleSlots(&algoGame.prefabManager);

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
                polyBlast_adjustSizeWeights(game, 0);
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

/**
    @brief Releases a shape at a specific board position.

    @param[in,out] shape        Pointer to the shape.
    @param[in]     pos          Target board position.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_releaseShapeAt(Shape_St *const shape, s8Vector2 pos, Board_St *const board) {
    shape->dragging = false;
    polyBlast_dragging = false;

    if (polyBlast_isShapeInBound(shape, board) && polyBlast_isShapePlaceable(shape, pos, board)) {
        polyBlast_placeShape(shape, castTo(u8Vector2) &pos, board);
        shape->placed = true;
        PlaySound(sound_shapePlacement);
    } else {
        shape->center = polyBlast_defaultPositions[shape->id];
    }
}

/**
    @brief Releases a dragged shape, attempting to place it.

    @param[in,out] shape        Pointer to the shape.
    @param[in,out] board        Pointer to the board.
*/
void polyBlast_releaseShape(Shape_St* const shape, Board_St* const board) {
    polyBlast_releaseShapeAt(shape, polyBlast_mapShapeToBoardPos(shape, board), board);
}
