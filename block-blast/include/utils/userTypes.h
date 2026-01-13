/**
 * @file userTypes.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Core type definitions used throughout the game.
 */

#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

/**
 * @brief Colors available for blocks in the game.
 */
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

/**
 * @brief Identifiers for base prefab shapes.
 *
 * These are used to index into the global prefabs array.
 */
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
    FONT8,
    FONT10, FONT12, FONT14, FONT16, FONT18,
    FONT20, FONT22, FONT24, FONT26, FONT28,
    FONT30, FONT32, FONT34, FONT36, FONT38,
    FONT40, FONT42, FONT44, FONT46, FONT48,
    _fontSizeCount
} FontSize_Et;

/**
 * @brief Variants of the prefab set (default, complete, extra, etc.).
 */
typedef enum {
    GAME_PREFAB_VARIANT_DEFAULT,
    GAME_PREFAB_VARIANT_COMPLETE,
    GAME_PREFAB_VARIANT_EXTRA,
    _gamePrefabVariantCount
} GamePrefabVariant_Et;

/**
 * @brief Represents a single block on the board. Can be refered as `unit`
 */
typedef struct {
    BlockColor_Et colorIndex;   ///< Color of the block.
    u8 hitsLeft;                ///< Remaining hits before the block is cleared (0 = empty).
} Block_St;

/**
 * @brief Static definition of a prefab shape.
 *
 * Describes the layout, possible rotations, and mirroring.
 */
typedef struct {
    u8 blockCount;                          ///< Number of blocks in the shape.
    s8 orientations;                        ///< Number of unique rotations (computed at init).
    bool8 canMirror;                        ///< Whether the shape has a distinct mirror variant.
    u8Vector2 offsets[MAX_SHAPE_SIZE];      ///< Relative offsets from center.
    u8 width, height;                       ///< Bounding box for quick collision checks.
} Prefab_St;

/**
 * @brief Dynamic array types for prefabs and indices.
 */
typeDA(Prefab_St, PrefabBag_St);
typeDA(u32, PrefabIndexBag_St);

/**
 * @brief Represents a prefab currently available to the player.
 */
typedef struct {
    const Prefab_St* prefab;    ///< Pointer to the static prefab definition.
    f32Vector2 center;          ///< Current screen position center.
    bool8 placed;               ///< Whether this shape has been placed this turn.
    BlockColor_Et colorIndex;   ///< Color assigned to this instance.
    bool8 dragging;             ///< Whether the player is currently dragging it.
    u8 id;                      ///< Slot ID (0-2).
} ActivePrefab_St;

/**
 * @brief Type alias for the three prefab slots shown to the player.
 */
typedef ActivePrefab_St PrefabSlots_t[3];

typedef struct {
    float weights[MAX_SHAPE_SIZE];  // index 0 = size 1, ..., index 8 = size 9
    float baseWeights[MAX_SHAPE_SIZE];
} SizeWeight_St;

/**
 * @brief The game board structure.
 */
typedef struct {
    Block_St blocks[BOARD_HEIGHT][BOARD_WIDTH];  ///< Row-major grid of blocks.
    f32Vector2 pos;                              ///< Screen position of top-left corner.
    u8 width, height;                            ///< Board dimensions (usually 8x8).
    bool8 *rowsToClear;                          ///< Temporary flags for clearing (allocated at init).
    bool8 *columnsToClear;                       ///< Temporary flags for clearing (allocated at init).
} Board_St;

/**
 * @brief Possible scene states for the application.
 */
typedef enum {
    SCENE_STATE_GAME,
    SCENE_STATE_ALL_PREFABS,
    _sceneStateCount
} SceneState_Et;

/**
 * @brief Complete game state suitable for saving/loading.
 */
typedef struct {
    Board_St board;                     ///< Current board state.
    PrefabSlots_t slots;                ///< Current three available prefabs.

    u64 score;                          ///< Player score.
    u8 streakCount;                     ///< Current combo streak.
    u8 streakPlacementResetCnt;
    char scoreText[32];                 ///< Pre-formatted score string for UI.
    char streakText[32];
    SizeWeight_St sizeWeights;

    bool8 gameOver;                     ///< Game over flag.
    SceneState_Et sceneState;           ///< Current scene/view.
    GamePrefabVariant_Et prefabVariant; ///< Which set of prefabs is active.
} GameState_St;

// typedef struct {
//     s8 board[UINT8_MAX];
//     u8 columnCount, rowCount;
// } Map_St;

#endif // USER_TYPES_H