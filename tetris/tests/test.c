#include <assert.h>

// Include le fichier principal (sans la fonction main)
#define main main_original
#include "../src/main.c"
#undef main

// Tests
void test_isColorsEqual() {
    printf("Test: isColorsEqual... ");
    
    Color c1 = {255, 0, 0, 255};
    Color c2 = {255, 0, 0, 255};
    Color c3 = {0, 255, 0, 255};
    
    assert(isColorsEqual(c1, c2) == true);
    assert(isColorsEqual(c1, c3) == false);
    
    printf("OK\n");
}

void test_areCoordinatesOOB() {
    printf("Test: areCoordinatesOOB... ");
    
    assert(areCoordinatesOOB(0, 0) == false);
    assert(areCoordinatesOOB(5, 10) == false);
    assert(areCoordinatesOOB(-1, 0) == true);
    assert(areCoordinatesOOB(10, 0) == true);
    assert(areCoordinatesOOB(0, 20) == true);
    
    printf("OK\n");
}

void test_initBoard() {
    printf("Test: initBoard... ");
    
    board_t board;
    initBoard(board);
    
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            assert(isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR));
        }
    }
    
    printf("OK\n");
}

void test_rotationCW() {
    printf("Test: rotationCW... ");
    
    boardShape_st shape;
    tetramino i_shape = I_SHAPE;
    memcpy(shape.shape, i_shape, sizeof(tetramino));
    shape.rotation = 0;
    
    // Position initiale
    assert(shape.shape[1].x == 0 && shape.shape[1].y == 0);
    
    rotationCW(&shape);
    
    // Après rotation
    assert(shape.rotation == 1);
    
    printf("OK\n");
}

void test_rotationCCW() {
    printf("Test: rotationCCW... ");
    
    boardShape_st shape;
    tetramino i_shape = I_SHAPE;
    memcpy(shape.shape, i_shape, sizeof(tetramino));
    shape.rotation = 0;
    
    rotationCCW(&shape);
    
    assert(shape.rotation == 3);
    
    printf("OK\n");
}

void test_isColliding() {
    printf("Test: isColliding... ");
    
    board_t board;
    initBoard(board);
    
    boardShape_st shape;
    tetramino i_shape = I_SHAPE;
    memcpy(shape.shape, i_shape, sizeof(tetramino));
    shape.position = (iVector2){5, 10};
    shape.color = (Color){255, 0, 0, 255};
    
    // Pas de collision
    assert(isColliding(board, shape) == false);
    
    // Ajouter un bloc
    board[10][5] = (Color){0, 255, 0, 255};
    assert(isColliding(board, shape) == true);
    
    printf("OK\n");
}

void test_putShapeInBoard() {
    printf("Test: putShapeInBoard... ");
    
    board_t board;
    initBoard(board);
    
    boardShape_st shape;
    tetramino o_shape = O_SHAPE;
    memcpy(shape.shape, o_shape, sizeof(tetramino));
    shape.position = (iVector2){4, 18};
    shape.color = (Color){255, 255, 0, 255};
    
    putShapeInBoard(board, shape);
    
    assert(isColorsEqual(board[18][4], shape.color));
    assert(isColorsEqual(board[18][5], shape.color));
    assert(isColorsEqual(board[19][4], shape.color));
    assert(isColorsEqual(board[19][5], shape.color));
    
    printf("OK\n");
}

void test_detectFullLines() {
    printf("Test: detectFullLines... ");
    
    board_t board;
    initBoard(board);
    int lineArray[4];
    int lineNb;
    
    // Remplir la ligne 19
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[19][x] = (Color){255, 0, 0, 255};
    }
    
    detectFullLines(board, lineArray, &lineNb);
    
    assert(lineNb == 1);
    assert(lineArray[0] == 19);
    
    printf("OK\n");
}

void test_copyBoard() {
    printf("Test: copyBoard... ");
    
    board_t src, dest;
    initBoard(src);
    
    src[5][3] = (Color){255, 0, 0, 255};
    src[10][7] = (Color){0, 255, 0, 255};
    
    copyBoard(src, dest);
    
    assert(isColorsEqual(dest[5][3], src[5][3]));
    assert(isColorsEqual(dest[10][7], src[10][7]));
    
    printf("OK\n");
}

int main() {
    printf("=== Tests unitaires Tetris ===\n\n");
    
    test_isColorsEqual();
    test_areCoordinatesOOB();
    test_initBoard();
    test_rotationCW();
    test_rotationCCW();
    test_isColliding();
    test_putShapeInBoard();
    test_detectFullLines();
    test_copyBoard();
    
    printf("\n=== Tous les tests sont passés! ===\n");
    
    return 0;
}