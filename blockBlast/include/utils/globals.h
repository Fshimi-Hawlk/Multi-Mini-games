#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "userTypes.h"

extern Rectangle windowRect;
extern Font      appFont;
extern Color     appBackgroundColor;
extern Font      fonts[MAX_FONT_COUNT];

extern const f32Vector2 defaultPositions[3];

extern const color32 blockColors[_blockColorCount];

extern const Prefab_St prefabs[];
extern const u32 prefabCount;

#endif // UTILS_GLOBALS_H