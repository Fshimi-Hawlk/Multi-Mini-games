/**
    @file userTypes.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Core type definitions used throughout the game.
*/
#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"
#include "APIs/generalAPI.h"

/**
    @brief Colors available for blocks in the game.
*/
typedef enum {
    BLOCK_COLOR_RED,        ///< Red color.
    BLOCK_COLOR_ORANGE,     ///< Orange color.
    BLOCK_COLOR_YELLOW,     ///< Yellow color.
    BLOCK_COLOR_GREEN,      ///< Green color.
    BLOCK_COLOR_CYAN,       ///< Cyan color.
    BLOCK_COLOR_BLUE,       ///< Blue color.
    BLOCK_COLOR_PURPLE,     ///< Purple color.
    BLOCK_COLOR_PINK,       ///< Pink color.
    _blockColorCount        ///< Total number of block colors.
} BlockColor_Et;

/**
    @brief Identifiers for base prefab shapes.

    These are used to index into the global prefabs array.
*/
enum {
    PREFAB_1x1,             ///< 1x1 block.
    PREFAB_1x2,             ///< 1x2 block.
    PREFAB_1x3,             ///< 1x3 block.
    PREFAB_L3,              ///< L-shape with 3 blocks.
    PREFAB_1x4,             ///< 1x4 block.
    PREFAB_2x2,             ///< 2x2 block.
    PREFAB_T,               ///< T-shape.
    PREFAB_L4,              ///< L-shape with 4 blocks.
    PREFAB_Z,               ///< Z-shape.
    PREFAB_1x5,             ///< 1x5 block.
    PREFAB_PLUS,            ///< Plus shape.
    PREFAB_L5,              ///< L-shape with 5 blocks.
    PREFAB_1x6,             ///< 1x6 block.
    PREFAB_2x3,             ///< 2x3 block.
    PREFAB_3x3,             ///< 3x3 block.
    _prefabNameCount        ///< Total number of prefabs.
};

/**
    @brief Variants of prefab sets used in the game.

    This enum determines which collection of prefabs is active during gameplay.
*/
typedef enum {
    GAME_PREFAB_VARIANT_DEFAULT,    ///< Basic set of shapes, suitable for standard play.
    GAME_PREFAB_VARIANT_COMPLETE,   ///< Includes all possible shapes and variants for variety.
    GAME_PREFAB_VARIANT_EXTRA,      ///< Additional experimental shapes.
    _gamePrefabVariantCount         ///< Sentinel for array sizing and looping, not a valid variant.
} GamePrefabVariant_Et;

/**
    @brief A single block on the board, holding its color and durability.

    Blocks are the basic units placed on the board.
*/
typedef struct {
    /**
        @brief Tracks how many times it needs to be part of a cleared line before removal.
    
        hitsLeft == 0: Empty space, ready for placement.
        hitsLeft > 0: Occupied, contributes to line clears.
        hitsLeft < 0: Not render block.
   */
    s8 hitsLeft;                ///< Remaining hits before the block is cleared (0 = empty).
    u8 colorIndex;              ///< Color index of the block.
} Block_St;

/**
    @brief Static blueprint for a shape (prefab), defining its block layout and transformations.

    Prefabs describe reusable shapes made of blocks. They're static data used to instance
    Shape_St during gameplay. The offsets are relative to the shape's center, and
    the bounding box (width/height) is computed for quick collision and placement checks.
    Orientations and mirroring are precomputed during init to avoid duplicates.
*/
typedef struct {
    u8 blockCount;                          ///< Number of blocks in the shape.
    s8 orientations;                        ///< Number of unique rotations (computed at init).
    bool canMirror;                         ///< Whether the shape has a distinct mirror variant.
    u8Vector2 offsets[MAX_SHAPE_SIZE];      ///< Relative offsets from center.
    u8 width, height;                       ///< Bounding box for quick collision checks.
} Prefab_St;

/**
    @brief Dynamic array (vector) holding a collection of prefabs.

    This is a growable container for Prefab_St, used as a "bag" to store all available
    shapes and their variants. It's populated at init and sampled randomly during play.
*/
typeDA(Prefab_St, PrefabBagVec_St);

/**
    @brief Dynamic array (vector) for prefab indices, grouped by block count.

    This holds indices into the prefabsBag, allowing quick random selection by size.
    Each "bag" corresponds to a shape size (e.g., bags[3] for 3-block shapes).
*/
typeDA(u32, PrefabIndexBagVec_St);

/**
    @brief An instance of a prefab that's active in the UI, ready for player interaction.

    This extends Prefab_St with runtime state like position, color, and interaction flags.
    Used in slots for dragging/placing. The id helps identify which slot it occupies.
*/
typedef struct {
    const Prefab_St* prefab;    ///< Pointer to the static prefab definition.
    f32Vector2 center;          ///< Current screen position center.
    bool placed;                ///< Whether this shape has been placed this turn.
    BlockColor_Et colorIndex;   ///< Color assigned to this instance.
    bool dragging;              ///< Whether the player is currently dragging it.
    u8 id;                      ///< Slot index (0-2).
} Shape_St;

/**
    @brief Type alias for the three shape slots shown to the player.
*/
typedef Shape_St ShapeSlots_t[3];

/**
    @brief Weights for random prefab selection based on shape size.

    This struct manages probabilities for picking shapes of different sizes.
    baseWeights are fixed starting values, while weights are adjusted dynamically
    during play to balance difficulty or variety.
*/
typedef struct {
    f32 baseWeights[MAX_SHAPE_SIZE];    ///< Initial probabilities per size.
    f32 runTimeWeights[MAX_SHAPE_SIZE]; ///< Runtime-adjusted weights.
} SizeWeight_St;

/**
    @brief All runtime data related to prefabs.

    Contains:
     - The complete bag of every prefab + all its unique rotations/mirrors
     - Per-size index bags used for fast weighted random selection
     - The three currently offered pieces (slots)
     - Dynamic size weights that get adjusted during gameplay

    This keeps everything that belongs together in one place and makes save/load
    or multiple game instances trivial in the future.
*/
typedef struct {
    PrefabIndexBagVec_St bags[MAX_SHAPE_SIZE];   ///< One bag per block count (size 1→9) containing indices into prefabsBag for O(1) weighted random picks.
    ShapeSlots_t slots;                         ///< The three shapes currently offered to the player.
    SizeWeight_St sizeWeights;                   ///< Runtime-adjusted weights for each size (used when picking the next shape).
} PrefabManager_St;

/**
    @brief The main game board: an 8×8 (or configurable) grid of blocks.

    The board uses row-major storage: blocks[y][x].
    Most fields are initialized once at game start
*/
typedef struct {
    /**
        Grid of blocks. Each cell holds color + durability.
        hitsLeft == 0  → empty tile
        hitsLeft <  0  → special/removed state (visual only, not clickable)
        hitsLeft >  0  → block exists, needs that many more clears
   */
    Block_St blocks[BOARD_HEIGHT][BOARD_WIDTH];
    
    /**
        Screen-space top-left corner of the board.
        Usually set once during layout and not changed unless UI is resized.
   */
    f32Vector2 pos;

    u8 width, height;       ///< Logical dimensions.

    /**
        Array of flags indicating which rows/columns are full and
        should be cleared at the end of the current placement.
   */
    bool *rowsToClear, *columnsToClear;
} Board_St;

/**
    @brief Dynamic array type alias for lists of empty board cell positions.

    Used by placement simulation and game-over detection to hold candidate
    anchor positions without repeated full-board scans.
*/
typeDA(u8Vector2, AnchorVec_St);

/**
    @brief Possible scene states for the application.
*/
typedef enum {
    SCENE_STATE_GAME,           ///< Gameplay scene.
    SCENE_STATE_ALL_PREFABS,    ///< Debug scene showing all prefabs.
    _sceneStateCount            ///< Total number of scenes.
} SceneState_Et;

/**
    @brief Score + combo/streak state with UI text cache.

    Updated via manageScoreAndStreak() -> buildScoreRelatedTexts().
    Text fields are performance optimization - only valid after formatting.
*/
typedef struct {
    u64  score;                 ///< Total score.
    u64  prevScore;             ///< Previous score

    u8   streakCount;           ///< Current combo length.
    
    /**
        @brief Remaining placements without a clear before streak resets.
        
        Refilled after clear: (streakCount + 1) / 2
        Decremented on non-clearing placements.
   */
    u8   streakGrace;
    char scoreText[32];         ///< "Score: %lu" formatted text.
    char streakText[32];        ///< "Streak: %u" formatted text.
} ScoringState_St;

/**
    @brief Prompt / modal state for save/load UI.
*/
typedef enum {
    PROMPT_NONE,                ///< No prompt active.
    PROMPT_START_LOAD,          ///< Startup: New Game vs Load Save.
    PROMPT_SAVE_QUIT,           ///< On window close: Save before leaving?
    PROMPT_SAVE_FILENAME,       ///< Filename input after choosing to save.
    PROMPT_SAVES_LIST,          ///< Scrollable save list table.
    PROMPT_CONFIRM_DELETE       ///< Confirmation before deleting a save.
} PromptState_Et;

/**
    @brief Core game state, encapsulating board, UI, and scoring.

    This struct is designed for easy saving/loading (e.g., via fwrite). It holds
    everything needed to resume a game: board layout, active shapes, score, and mode.
    The scoreText and streakText are pre-formatted for quick UI drawing.
*/
typedef struct {
    Board_St board;                     ///< Current board state.
    ScoringState_St scoring;            ///< Scoring and streak state.
    PrefabManager_St prefabManager;     ///< All prefab-related data.

    bool gameOver;                      ///< Game over flag.
    SceneState_Et sceneState;           ///< Current scene/view.

    bool hasBeenLost;                   ///< Anti-cheat: true if this save already reached gameOver once.
    const char *loadFilename;           ///< Filename to load game from.
} PolyBlastGame_St;

#endif // USER_TYPES_H