/**
    @file globals.h
    @author Kimi BERGE
    @date 2026-01-07
    @date 2026-04-14
    @brief Global variables and resources.
*/
#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "utils/userTypes.h"
#include "sharedWidgets/types.h"

/**
    @brief Global application state and resources.

    All extern variables used throughout the game.
*/

extern Font      polyBlast_fonts[__fontSizeCount]; ///< Array of loaded fonts in increasing sizes.

extern GameState_St polyBlast_mainGameState;       ///< Main game state containing board, score, etc.

extern GamePrefabVariant_Et polyBlast_prefabVariant;          ///< Which set of prefabs is active.
extern u32 polyBlast_prefabsPerSizeOffsets[MAX_SHAPE_SIZE];   ///< Array recording the starting indices whenever the blockCount changes.
extern PrefabBagVec_St polyBlast_prefabsBag;                  ///< Complete bag of every prefab variant (rotations + mirrors).
extern Shape_St *polyBlast_shapeBag;                           ///< Temporary array used when viewing all prefabs.

extern f32Vector2 polyBlast_mouseDeltaFromShapeCenter;    ///< Offset from shape center when dragging.
extern bool polyBlast_dragging;                           ///< Global flag indicating if any shape is being dragged.

extern const f32Vector2 polyBlast_defaultPositions[3];    ///< Default screen positions for the three slots.

extern const Color polyBlast_blockColors[_blockColorCount]; ///< Color palette for each BlockColor_Et.

extern const Prefab_St polyBlast_prefabs[];               ///< Static array of base prefab definitions.
extern const u32 polyBlast_prefabCount;                   ///< Number of entries in the prefabs[] array.

extern Sound sound_shapePlacement;                        ///< Sound played on successful shape placement.
extern Sound sound_combo;                                 ///< Sound played on scoring a combo/streak.
extern Sound sound_gameOver;                              ///< Sound played on game over.
extern Sound sound_lineBreak;                             ///< Sound played on clearing a line.

extern PromptState_Et polyBlast_currentPrompt;                      ///< Modal state for save/load UI.

#endif // UTILS_GLOBALS_H