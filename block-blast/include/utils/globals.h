/**
 * @file globals.h
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Global variables and resources.
 */

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

/**
 * @brief Global application state and resources.
 *
 * All extern variables used throughout the game.
 */

#include "userTypes.h"

extern Rectangle windowRect;            ///< Main window rectangle (set at init).
extern Font      appFont;               ///< Primary font for UI text.
extern Font      fonts[_fontSizeCount]; ///< Array of loaded fonts in increasing sizes.

extern PrefabBagVec_St prefabsBag;                  ///< Bag containing all available prefabs and variants.
extern PrefabIndexBagVec_St bags[MAX_SHAPE_SIZE];   ///< Bags for random prefab selection by block count.
extern ActivePrefab_St *shapeBag;                   ///< Temporary array used when viewing all prefabs (SCENE_STATE_ALL_PREFABS).

extern u32 prefabsPerSizeOffsets[MAX_SHAPE_SIZE]; ///< array to record the starting indexes whenever the blockCount changes

extern GameState_St game;       ///< Main game state.

extern f32Vector2 mouseDeltaFromShapeCenter;    ///< Offset from shape center when dragging.
extern bool8 dragging;                          ///< Global flag indicating if any shape is being dragged.

extern const f32Vector2 defaultPositions[3]; ///< Default screen positions for the three slots.

extern const color32 blockColors[_blockColorCount]; ///< Color palette for each BlockColor_Et.

extern const Prefab_St prefabs[];  ///< Static array of base prefab definitions.
extern const u32 prefabCount;      ///< Number of entries in the prefabs[] array.

#endif // UTILS_GLOBALS_H