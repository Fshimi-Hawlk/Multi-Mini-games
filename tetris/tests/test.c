#include "core/board.h"
#include "core/shape.h"
#include <assert.h>

void test_tetris_areCoordinatesOOB(void);
void test_tetris_tetris_isCollidingAt(void);
void test_tetris_putShapeInBoard(void);
void test_tetris_detectFullLines(void);

void test_tetris_rotationCW(void);
void test_tetris_rotationCCW(void);

int main(void) {
    test_tetris_areCoordinatesOOB();
    test_tetris_tetris_isCollidingAt();
    test_tetris_putShapeInBoard();
    test_tetris_detectFullLines();

    test_tetris_rotationCW();
    test_tetris_rotationCCW();

    return 0;
}

void test_tetris_areCoordinatesOOB(void) {
    printf("=== Test tetris_areCoordinatesOOB ===\n");
    
    // Test coordonnées valides (dans les limites)
    assert(!tetris_areCoordinatesOOB(0, 0));
    printf("✓ Test (0, 0) : valide\n");
    
    assert(!tetris_areCoordinatesOOB(BOARD_WIDTH - 1, BOARD_HEIGHT - 1));
    printf("✓ Test (%d, %d) : valide\n", BOARD_WIDTH - 1, BOARD_HEIGHT - 1);
    
    assert(!tetris_areCoordinatesOOB(5, 5));
    printf("✓ Test (5, 5) : valide\n");
    
    // Test coordonnées invalides (hors limites)
    assert(tetris_areCoordinatesOOB(-1, 5));
    printf("✓ Test (-1, 5) : hors limites (x < 0)\n");
    
    assert(tetris_areCoordinatesOOB(BOARD_WIDTH, 5));
    printf("✓ Test (%d, 5) : hors limites (x >= BOARD_WIDTH)\n", BOARD_WIDTH);
    
    assert(tetris_areCoordinatesOOB(5, BOARD_HEIGHT));
    printf("✓ Test (5, %d) : hors limites (y >= BOARD_HEIGHT)\n", BOARD_HEIGHT);
    
    assert(tetris_areCoordinatesOOB(-1, BOARD_HEIGHT));
    printf("✓ Test (-1, %d) : hors limites (x < 0 ET y >= BOARD_HEIGHT)\n", BOARD_HEIGHT);
    
    printf("\n=== Tous les tests sont passés! ===\n\n");
}

void test_tetris_tetris_isCollidingAt(void) {
    printf("=== Test tetris_tetris_isCollidingAt ===\n");
    board_t board;
    tetris_initBoard(board);
    
    // Créer une forme simple (carré 2x2)
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        .position = {0, 0},
        .color = {255, 0, 0, 255}
    };
    
    // Test 1 : pas de collision sur plateau vide
    assert(!tetris_tetris_isCollidingAt(board, shape, (iVector2){0, 0}));
    printf("✓ Pas de collision sur plateau vide\n");
    
    // Test 2 : placer une forme et vérifier collision
    tetris_putShapeInBoard(board, shape);
    assert(tetris_tetris_isCollidingAt(board, shape, (iVector2){0, 0}));
    printf("✓ Collision détectée avec forme placée\n");
    
    // Test 3 : pas de collision si on se décale
    assert(!tetris_tetris_isCollidingAt(board, shape, (iVector2){5, 5}));
    printf("✓ Pas de collision en position décalée\n");
    
    printf("=== Tous les tests tetris_tetris_isCollidingAt sont passés! ===\n\n");
}

void test_tetris_putShapeInBoard(void) {
    printf("=== Test tetris_putShapeInBoard ===\n");
    board_t board;
    tetris_initBoard(board);
    
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {1, 1}},
        .position = {3, 5},
        .color = {100, 150, 200, 255}
    };
    
    // Vérifier que le plateau est vide avant
    assert(ColorIsEqual(board[5][3], BOARD_BACKGROUND_COLOR));
    printf("✓ Plateau vide avant insertion\n");
    
    // Insérer la forme
    tetris_putShapeInBoard(board, shape);
    
    // Vérifier que la forme est placée
    assert(!ColorIsEqual(board[5][3], BOARD_BACKGROUND_COLOR));
    assert(ColorIsEqual(board[5][3], shape.color));
    printf("✓ Forme correctement placée au plateau\n");
    
    // Vérifier que tous les 4 blocs sont placés
    assert(ColorIsEqual(board[5][4], shape.color));
    assert(ColorIsEqual(board[6][3], shape.color));
    assert(ColorIsEqual(board[6][4], shape.color));
    printf("✓ Les 4 blocs sont correctement positionnés\n");
    
    printf("=== Tous les tests tetris_putShapeInBoard sont passés! ===\n\n");
}

void test_tetris_detectFullLines(void) {
    printf("=== Test tetris_detectFullLines ===\n");
    board_t board;
    tetris_initBoard(board);
    
    Color testColor = {255, 255, 255, 255};
    
    // Test 1 : plateau vide, aucune ligne complète
    int lineArray[4] = {0};
    int lineNb = 0;
    tetris_detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 0);
    printf("✓ Plateau vide : 0 ligne complète\n");
    
    // Test 2 : remplir une ligne complètement
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[10][x] = testColor;
    }
    lineNb = 0;
    tetris_detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 1);
    assert(lineArray[0] == 10);
    printf("✓ Une ligne complète détectée à y=10\n");
    
    // Test 3 : remplir 2 lignes
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[15][x] = testColor;
    }
    lineNb = 0;
    tetris_detectFullLines(board, lineArray, &lineNb);
    assert(lineNb == 2);
    printf("✓ Deux lignes complètes détectées\n");
    
    printf("=== Tous les tests tetris_detectFullLines sont passés! ===\n\n");
}

void test_tetris_rotationCW(void) {
    printf("=== Test tetris_rotationCW ===\n");
    
    // Créer une forme en L (pas symétrique pour voir la rotation)
    boardShape_st shape = {
        .shape = {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
        .position = {0, 0},
        .color = {255, 0, 0, 255},
        .rotation = 0
    };
    
    // Sauvegarder l'état initial
    iVector2 original[4];
    for (int i = 0; i < 4; i++) {
        original[i] = shape.shape[i];
    }
    
    // Test 1 : rotation horaire
    tetris_rotationCW(&shape);
    assert(shape.rotation == 1);
    printf("✓ Rotation compteur à 1\n");
    
    // Vérifier que au moins une coordonnée a changé
    bool changed = false;
    for (int i = 0; i < 4; i++) {
        if (shape.shape[i].x != original[i].x || shape.shape[i].y != original[i].y) {
            changed = true;
            break;
        }
    }
    assert(changed);
    printf("✓ Coordonnées modifiées après rotation\n");
    
    // Test 2 : 4 rotations = retour à l'original
    tetris_rotationCW(&shape);
    tetris_rotationCW(&shape);
    tetris_rotationCW(&shape);
    assert(shape.rotation == 0);
    printf("✓ 4 rotations CW = rotation complète\n");
    
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ Coordonnées revenues à l'original après 4 rotations\n");
    
    printf("=== Tous les tests tetris_rotationCW sont passés! ===\n\n");
}

void test_tetris_rotationCCW(void) {
    printf("=== Test tetris_rotationCCW ===\n");
    
    // Créer une forme en L (pas symétrique)
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
    
    // Test 1 : rotation counter-clockwise
    tetris_rotationCCW(&shape);
    assert(shape.rotation == 3);
    printf("✓ Rotation compteur à 3\n");
    
    // Test 2 : 4 rotations CCW = retour à l'original
    tetris_rotationCCW(&shape);
    tetris_rotationCCW(&shape);
    tetris_rotationCCW(&shape);
    assert(shape.rotation == 0);
    printf("✓ 4 rotations CCW = rotation complète\n");
    
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ Coordonnées revenues à l'original après 4 rotations\n");
    
    // Test 3 : CW + CCW = identité
    tetris_rotationCW(&shape);
    tetris_rotationCCW(&shape);
    assert(shape.rotation == 0);
    for (int i = 0; i < 4; i++) {
        assert(shape.shape[i].x == original[i].x);
        assert(shape.shape[i].y == original[i].y);
    }
    printf("✓ CW suivi de CCW ramène à l'original\n");
    
    printf("=== Tous les tests tetris_rotationCCW sont passés! ===\n\n");
}
