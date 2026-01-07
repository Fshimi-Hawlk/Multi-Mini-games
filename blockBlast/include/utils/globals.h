#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

extern Rectangle windowRect;
extern Font      appFont;
extern Font      fonts[MAX_FONT_COUNT];

extern PrefabBag_St prefabsBag;
extern PrefabIndexBag_St bags[MAX_BLOCK_PER_SHAPE];
extern ActivePrefab_St *shapeBag;

// array to record the starting indexes whenever the blockCount changes
extern u32 prefabsPerSizeOffsets[MAX_BLOCK_PER_SHAPE];

extern GameState_St game;
extern GameState_St algoGame;

extern f32Vector2 mouseDeltaFromShapeCenter;
extern bool8 dragging;

extern const f32Vector2 defaultPositions[3];

extern const color32 blockColors[_blockColorCount];

extern const Prefab_St prefabs[];
extern const u32 prefabCount;

#endif // UTILS_GLOBALS_H