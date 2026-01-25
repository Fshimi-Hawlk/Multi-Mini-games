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
 * @brief Variants of prefab sets used in the game.
 *
 * This enum determines which collection of prefabs is active during gameplay.
 */
typedef enum {
    GAME_PREFAB_VARIANT_DEFAULT,    ///< Basic set of shapes, suitable for standard play.
    GAME_PREFAB_VARIANT_COMPLETE,   ///< Includes all possible shapes and variants for variety.
    GAME_PREFAB_VARIANT_EXTRA,      ///< Additional experimental shapes.
    _gamePrefabVariantCount         ///< sentinel for array sizing and looping, not a valid variant.
} GamePrefabVariant_Et;

/**
 * @brief A single block on the board, holding its color and durability.
 *
 * Blocks are the basic units placed on the board.
 */
typedef struct {
    BlockColor_Et colorIndex;   ///< Color of the block.

    /**
     * @brief Tracks how many times it needs to be part of a cleared line before removal.
     *
     * hitsLeft == 0: Empty space, ready for placement.
     * hitsLeft > 0: Occupied, contributes to line clears.
     * hitsLeft < 0: Special state, like visually faded or removed but still drawn for effects.
     */
    u8 hitsLeft;                ///< Remaining hits before the block is cleared (0 = empty).
} Block_St;

/**
 * @brief Static blueprint for a shape (prefab), defining its block layout and transformations.
 *
 * Prefabs describe reusable shapes made of blocks. They're static data used to instance
 * ActivePrefab_St during gameplay. The offsets are relative to the shape's center, and
 * the bounding box (width/height) is computed for quick collision and placement checks.
 * Orientations and mirroring are precomputed during init to avoid duplicates.
 */
typedef struct {
    u8 blockCount;                          ///< Number of blocks in the shape.
    s8 orientations;                        ///< Number of unique rotations (computed at init).
    bool8 canMirror;                        ///< Whether the shape has a distinct mirror variant.
    u8Vector2 offsets[MAX_SHAPE_SIZE];      ///< Relative offsets from center.
    u8 width, height;                       ///< Bounding box for quick collision checks.
} Prefab_St;

/**
 * @brief Dynamic array (vector) holding a collection of prefabs.
 *
 * This is a growable container for Prefab_St, used as a "bag" to store all available
 * shapes and their variants. It's populated at init and sampled randomly during play.
 */
typeDA(Prefab_St, PrefabBagVec_St);

/**
 * @brief Dynamic array (vector) for prefab indices, grouped by block count.
 *
 * This holds indices into the prefabsBag, allowing quick random selection by size.
 * Each "bag" corresponds to a shape size (e.g., bags[3] for 3-block shapes).
 */
typeDA(u32, PrefabIndexBagVec_St);

/**
 * @brief An instance of a prefab that's active in the UI, ready for player interaction.
 *
 * This extends Prefab_St with runtime state like position, color, and interaction flags.
 * Used in slots for dragging/placing. The id helps identify which slot it occupies.
 */
typedef struct {
    const Prefab_St* prefab;    ///< Pointer to the static prefab definition.
    f32Vector2 center;          ///< Current screen position center.
    bool8 placed;               ///< Whether this shape has been placed this turn.
    BlockColor_Et colorIndex;   ///< Color assigned to this instance.
    bool8 dragging;             ///< Whether the player is currently dragging it.
    u8 id;                      ///< Slot index (0-2).
} ActivePrefab_St;

/**
 * @brief Type alias for the three prefab slots shown to the player.
 */
typedef ActivePrefab_St PrefabSlots_t[3];

/**
 * @brief Weights for random prefab selection based on shape size.
 *
 * This struct manages probabilities for picking shapes of different sizes.
 * baseWeights are fixed starting values, while weights are adjusted dynamically
 * during play to balance difficulty or variety.
 */
typedef struct {
    f32 baseWeights[MAX_SHAPE_SIZE];    ///< Initial probabilities per size
    f32 weights[MAX_SHAPE_SIZE];        ///< Runtime-adjusted weights
} SizeWeight_St;

/**
 * @brief The main game board: an 8×8 (or configurable) grid of blocks.
 *
 * The board uses row-major storage: blocks[y][x].
 * Most fields are initialized once at game start
 */
typedef struct {
    /**
     * Grid of blocks. Each cell holds color + durability.
     * hitsLeft == 0  → empty tile
     * hitsLeft <  0  → special/removed state (visual only, not clickable)
     * hitsLeft >  0  → block exists, needs that many more clears
     */
    Block_St blocks[BOARD_HEIGHT][BOARD_WIDTH];
    
    /**
     * Screen-space top-left corner of the board.
     * Usually set once during layout and not changed unless UI is resized.
     */
    f32Vector2 pos;

    u8 width, height;       ///< Logical dimensions

    /**
     * Array of flags indicating which rows/columns are full and
     * should be cleared at the end of the current placement.
     */
    bool8 *rowsToClear, *columnsToClear;
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
 * @brief Core game state, encapsulating board, UI, and scoring.
 *
 * This struct is designed for easy saving/loading (e.g., via fwrite). It holds
 * everything needed to resume a game: board layout, active shapes, score, and mode.
 * The scoreText and streakText are pre-formatted for quick UI drawing.
 */
typedef struct {
    Board_St board;                     ///< Current board state.
    PrefabSlots_t slots;                ///< Current three available prefabs.

    u64 score;                          ///< Player score.
    u8 streakCount;                     ///< Current combo streak.
    u8 streakPlacementResetCnt;         ///< Prefab placement counter to reset streak under a specific threshold
    char scoreText[32];                 ///< Formatted score string for UI.
    char streakText[32];                ///< Formatted streak string for UI.
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