#include "core/board.h"
#include "core/shape.h"
#include <assert.h>

void test_areCoordinatesOOB(void);
void test_isCollidingAt(void);
void test_putShapeInBoard(void);
void test_detectFullLines(void);

void test_rotationCW(void);
void test_rotationCCW(void);

int main(void) {
    test_areCoordinatesOOB();
    test_isCollidingAt();
    test_putShapeInBoard();
    test_detectFullLines();

    test_rotationCW();
    test_rotationCCW();

    return 0;
}

void test_areCoordinatesOOB(void) {
    printf("=== Test areCoordinatesOOB ===\n");
    
    // Test valid coordinates (within bounds)
    assert(!areCoordinatesOOB(0, 0));
    printf("✓ Test (0, 0): valid\n");
    
    assert(!areCoordinatesOOB(BOARD_WIDTH - 1, BOARD_HEIGHT - 1));
    printf("✓ Test (%d, %d): valid\n", BOARD_WIDTH - 1, BOARD_HEIGHT - 1);
    
    assert(!areCoordinatesOOB(5, 5));
    printf("✓ Test (5, 5): valid\n");
    
    // Test invalid coordinates (out of bounds)
    assert(areCoordinatesOOB(-1, 5));
    printf("✓ Test (-1, 5): out of bounds (x < 0)\n");
    
    assert(areCoordinatesOOB(BOARD_WIDTH, 5));
    printf("✓ Test (%d, 5): out of bounds (x >= BOARD_WIDTH)\n", BOARD_WIDTH);
    
    assert(areCoordinatesOOB(5, BOARD_HEIGHT));
    printf("✓ Test (5, %d): out of bounds (y >= BOARD_HEIGHT)\n", BOARD_HEIGHT);
    
    assert(areCoordinatesOOB(-1, BOARD_HEIGHT));
    printf("✓ Test (-1, %d): out of bounds (x < 0 AND y >= BOARD_HEIGHT)\n", BOARD_HEIGHT);
    
    printf("\n=== All tests passed! ===\n\n");
}

void test_isCollidingAt(void) {
    printf("=== Test isCollidingAt ===\n");
    board_t board;
    initBoard(board);
    
    // Create a simple shape (2x2 square)
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        .position = {0, 0},
        .color = {255, 0, 0, 255}
    };
    
    // Test 1: no collision on empty board
    assert(!isCollidingAt(board, shape, (iVector2){0, 0}));
    printf("✓ No collision on empty board\n");
    
    // Test 2: place a shape and verify collision
    putShapeInBoard(board, shape);
    assert(isCollidingAt(board, shape, (iVector2){0, 0}));
    printf("✓ Collision detected with placed shape\n");
    
    // Test 3: no collision if we shift position
    assert(!isCollidingAt(board, shape, (iVector2){5, 5}));
    printf("✓ No collision at shifted position\n");
    
    printf("=== All isCollidingAt tests passed! ===\n\n");
}

void test_putShapeInBoard(void) {
    printf("=== Test putShapeInBoard ===\n");
    board_t board;
    initBoard(board);
    
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        .position = {3, 5},
        .color = {100, 150, 200, 255}
    };
    
    // Verify board is empty before
    assert(ColorIsEqual(board[5][3], BOARD_BACKGROUND_COLOR));
    printf("✓ Board empty before insertion\n");
    
    // Insert the shape
    putShapeInBoard(board, shape);
    
    // Verify the shape is placed
    assert(!ColorIsEqual(board[5][3], BOARD_BACKGROUND_COLOR));
    assert(ColorIsEqual(board[5][3], shape.color));
    printf("✓ Shape correctly placed on board\n");
    
    // Verify all 4 blocks are placed
    assert(ColorIsEqual(board[5][4], shape.color));
    assert(ColorIsEqual(board[6][3], shape.color));
    assert(ColorIsEqual(board[6][4], shape.color));
    printf("✓ All 4 blocks are correctly positioned\n");
    
    printf("=== All putShapeInBoard tests passed! ===\n\n");
}

void test_detectFullLines(void) {
    printf("=== Test detectFullLines ===\n");
    board_t board;
    initBoard(board);
    
    Color testColor = {255, 255, 255, 255};
    
    // Test 1: empty board, no complete lines
    int lineArray[4] = {0};
    int lineNb = 0;
    detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 0);
    printf("✓ Empty board: 0 complete lines\n");
    
    // Test 2: fill one line completely
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[10][x] = testColor;
    }
    lineNb = 0;
    detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 1);
    assert(lineArray[0] == 10);
    printf("✓ One complete line detected at y=10\n");
    
    // Test 3: fill 2 lines
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[15][x] = testColor;
    }
    lineNb = 0;
    detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 2);
    printf("✓ Two complete lines detected\n");
    
    printf("=== All detectFullLines tests passed! ===\n\n");
}

void test_rotationCW(void) {
    printf("=== Test rotationCW ===\n");
    
    // Create L-shape (asymmetric to see rotation)
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
        .position = {0, 0},
        .color = {255, 0, 0, 255},
        .rotation = 0
    };
    
    // Save initial state
    iVector2 original[4];
    for (int i = 0; i < 4; i++) {
        original[i] = shape.shape[i];
    }
    
    // Test 1: clockwise rotation
    rotationCW(&shape);
    assert(shape.rotation == 1);
    printf("✓ Rotation counter at 1\n");
    
    // Verify at least one coordinate changed
    bool changed = false;
    for (int i = 0; i < 4; i++) {
        if (shape.shape[i].x != original[i].x || shape.shape[i].y != original[i].y) {
            changed = true;
            break;
        }
    }
    assert(changed);
    printf("✓ Coordinates modified after rotation\n");
    
    // Test 2: 4 rotations = return to original
    rotationCW(&shape);
    rotationCW(&shape);
    rotationCW(&shape);
    assert(shape.rotation == 0);
    printf("✓ 4 CW rotations = full rotation\n");
    
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ Coordinates back to original after 4 rotations\n");
    
    printf("=== All rotationCW tests passed! ===\n\n");
}

void test_rotationCCW(void) {
    printf("=== Test rotationCCW ===\n");
    
    // Create L-shape (asymmetric)
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
        .position = {0, 0},
        .color = {0, 255, 0, 255},
        .rotation = 0
    };
    
    iVector2 original[4];
    for (int i = 0; i < 4; i++) {
        original[i] = shape.shape[i];
    }
    
    // Test 1: counter-clockwise rotation
    rotationCCW(&shape);
    assert(shape.rotation == 3);
    printf("✓ Rotation counter at 3\n");
    
    // Test 2: 4 CCW rotations = return to original
    rotationCCW(&shape);
    rotationCCW(&shape);
    rotationCCW(&shape);
    assert(shape.rotation == 0);
    printf("✓ 4 CCW rotations = full rotation\n");
    
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ Coordinates back to original after 4 rotations\n");
    
    // Test 3: CW + CCW = identity
    rotationCW(&shape);
    rotationCCW(&shape);
    assert(shape.rotation == 0);
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ CW followed by CCW returns to original\n");
    
    printf("=== All rotationCCW tests passed! ===\n\n");
}
