/**
 * @file test_shape_placement.c
 * @brief Unit tests for shape validation and placement.
 */

#include "core/game/shape.h"
#include "utils/common.h"
#include "utils/configs.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

#include <assert.h>

static void test_shape_in_bounds(void) {
    Board_St mockBoard = {
        .width = 8,
        .height = 8,
    };

    f32Vector2 mockBoardPos = {
        .x = WINDOW_WIDTH / 2.0f,
        .y = WINDOW_HEIGHT / 3.0f
    };

    f32Vector2 mockBoardPxSize = {
        .x = BLOCK_PX_SIZE * mockBoard.width,
        .y = BLOCK_PX_SIZE * mockBoard.height,
    };

    mockBoard.pos = (f32Vector2) {
        .x = mockBoardPos.x - mockBoardPxSize.x / 2.0f ,
        .y = mockBoardPos.y - mockBoardPxSize.y / 2.0f
    };

    ActivePrefab_St mockShape = {
        .center = mockBoard.pos,
        .prefab = &prefabs[PREFAB_1x1],
    };

    assert(isShapeInBound(&mockShape, &mockBoard) == true);
    log_info("OK");

    mockShape.center.x = -10;
    assert(isShapeInBound(&mockShape, &mockBoard) == false);
    log_info("OK");
}

static void test_can_place_at(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    ActivePrefab_St mockShape = {0};
    mockShape.prefab = &prefabs[PREFAB_1x1];
    s8Vector2 pos = {0, 0};

    assert(isShapePlaceable(&mockShape, pos, &testBoard) == true);
    log_info("OK");

    testBoard.blocks[0][0].hitsLeft = 1; // Occupied
    assert(isShapePlaceable(&mockShape, pos, &testBoard) == false);
    log_info("OK");
}

static void test_place_shape(void) {
    Board_St testBoard = {.width = 8, .height = 8};
    ActivePrefab_St mockShape = {
        .prefab = &prefabs[PREFAB_1x2],
        .colorIndex = 0
    };

    u8Vector2 pos = {0, 0};

    placeShape(&mockShape, pos, &testBoard);
    assert(testBoard.blocks[0][0].hitsLeft == 1);
    log_info("OK");
    assert(testBoard.blocks[1][0].hitsLeft == 1);
    log_info("OK");
    assert(testBoard.blocks[0][0].colorIndex == 0);
    log_info("OK");
}

int main(void) {
    // Assume prefabs initialized
    test_shape_in_bounds();
    test_can_place_at();
    test_place_shape();
    log_info("Shape placement tests passed");
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"

#define CONTEXT_ARENA_IMPLEMENTATION
#include "contextArena.h"

#define RAND_IMPLEMENTATION
#include "rand.h"