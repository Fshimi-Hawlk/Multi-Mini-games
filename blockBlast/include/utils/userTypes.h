#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

typedef enum {
    BLOCK_COLOR_RED,
    BLOCK_COLOR_ORANGE,
    BLOCK_COLOR_YELLOW,
    BLOCK_COLOR_GREEN,
    BLOCK_COLOR_CYAN,
    BLOCK_COLOR_BLUE,
    BLOCK_COLOR_PURPLE,
    BLOCK_COLOR_PINK,
    _blockColorCount
} BlockColor_Et;

enum {
    PREFAB_1x1,
    PREFAB_1x2,
    PREFAB_1x3,
    PREFAB_L3,
    PREFAB_1x4,
    PREFAB_2x2,
    PREFAB_T,
    PREFAB_L4,
    PREFAB_Z,
    PREFAB_1x5,
    PREFAB_PLUS,
    PREFAB_L5,
    PREFAB_1x6,
    PREFAB_2x3,
    PREFAB_3x3,
    _prefabNameCount
};

typedef enum {
    GAME_PREFAB_VARIANT_DEFAULT,
    GAME_PREFAB_VARIANT_COMPLETE,
    GAME_PREFAB_VARIANT_EXTRA,
    _gamePrefabVariantCount
} GamePrefabVariant_Et;

typedef struct {
    BlockColor_Et colorIndex;
    u8 hitsLeft;
} Block_St;

typedef struct {
    u8 blockCount;
    s8 orientations;
    bool8 canMirror;
    u8Vector2 offsets[MAX_BLOCK_PER_SHAPE];
    u8 width, height; // bounding box (for quick collision checks)
} Prefab_St;

typeDA(Prefab_St, PrefabBag_St);
typeDA(u32, PrefabIndexBag_St);


typedef struct {
    const Prefab_St* prefab;
    f32Vector2 center;
    bool8 placed;
    BlockColor_Et colorIndex;
    bool8 dragging;
} ActivePrefab_St;

typedef ActivePrefab_St PrefabSlots_t[3];

typedef struct {
    Block_St blocks[BOARD_HEIGHT][BOARD_WIDTH];  // row-major: blocks[y][x]
    f32Vector2 pos;
    u8 width, height; // top-left corner
    bool8 *rowsToClear;
    bool8 *columnsToClear;
} Board_St;

typedef enum {
    SCENE_STATE_GAME,
    SCENE_STATE_ALL_PREFABS,
    _sceneStateCount
} SceneState_Et;

// For saves: serialize this (e.g., via fwrite).
typedef struct {
    Board_St board;
    PrefabSlots_t slots;
    
    u64 score;
    u8 streakCount;
    char scoreText[32];

    bool8 gameOver;
    SceneState_Et sceneState;
    GamePrefabVariant_Et prefabVariant;
} GameState_St;

// typedef struct {
//     s8 board[UINT8_MAX];
//     u8 columnCount, rowCount;
// } Map_St;

#endif // USER_TYPES_H