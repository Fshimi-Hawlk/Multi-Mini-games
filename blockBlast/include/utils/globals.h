#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "utils/common.h"

extern Rectangle windowRect;
extern Font      appFont;
extern Color     appBackgroundColor;
extern Font      fonts[MAX_FONT_COUNT];

extern Camera3D camera;

extern bool     cursorVisible;

extern float    cameraYaw;
extern float    cameraPitch;

extern const float MOVE_SPEED;
extern const float RUN_MULTIPLIER;

#endif // UTILS_GLOBALS_H