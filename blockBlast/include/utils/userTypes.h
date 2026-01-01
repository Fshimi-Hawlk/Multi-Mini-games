#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

typedef enum {
    CELL_COLOR_RED,
    CELL_COLOR_ORANGE,
    CELL_COLOR_YELLOW,
    CELL_COLOR_GREEN,
    CELL_COLOR_CYAN,
    CELL_COLOR_BLUE,
    CELL_COLOR_PURPLE,
    CELL_COLOR_PINK,
    _cellColorCount
} CellColor_Et;

typedef struct {
    u8 cellCount;
    u8 DOC; // Distinct Orientation Count
    bool8 canMirror;
    s8Vector2 relPos[MAX_BLOCK_PER_SHAPE];
} PrefabData_St;

typedef struct {
    CellColor_Et colorIndex;
    u8 hitsLeft;
} Cell_St;

typedef struct {
    Cell_St cells[MAX_BLOCK_PER_SHAPE];
    const PrefabData_St* prefabData; // reference to the prefabs array
    u8 width, height; // bounding box (for quick collision checks)
} Prefab_St;

typedef struct {
    const Prefab_St* prefab;
    f32Vector2 pos;
    bool8 placed;
} ActivePrefab_St;

typedef ActivePrefab_St PrefabSlots_t[3];

typedef struct {
    Cell_St cells[BOARD_HEIGHT][BOARD_WIDTH];  // row-major: cells[y][x]
} Board_St;


#endif // USER_TYPES_H