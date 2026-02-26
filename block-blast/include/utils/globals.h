/**
    @file globals.h
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Global variables and resources.
*/

#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

/**
    @brief Global application state and resources.

    All extern variables used throughout the game.
*/

#include "userTypes.h"

extern Rectangle windowRect;            ///< Main window rectangle (set at init).
extern Font      appFont;               ///< Primary font for UI text.
extern Font      fonts[_fontSizeCount]; ///< Array of loaded fonts in increasing sizes.

extern GameState_St mainGameState;
extern GameState_St previousGameState;

extern GamePrefabVariant_Et prefabVariant;          ///< Which set of prefabs is active.
extern u32 prefabsPerSizeOffsets[MAX_SHAPE_SIZE];   ///< array to record the starting indexes whenever the blockCount changes
extern PrefabBagVec_St prefabsBag;                  ///< Complete bag of every prefab variant (rotations + mirrors when applicable).
extern Shape_St *shapeBag;                   ///< Temporary array used when viewing all prefabs (SCENE_STATE_ALL_PREFABS).

extern f32Vector2 mouseDeltaFromShapeCenter;    ///< Offset from shape center when dragging.
extern bool dragging;                          ///< Global flag indicating if any shape is being dragged.

extern const f32Vector2 defaultPositions[3]; ///< Default screen positions for the three slots.

extern const Color blockColors[_blockColorCount]; ///< Color palette for each BlockColor_Et.

extern const Prefab_St prefabs[];  ///< Static array of base prefab definitions.
extern const u32 prefabCount;      ///< Number of entries in the prefabs[] array.

#endif // UTILS_GLOBALS_H