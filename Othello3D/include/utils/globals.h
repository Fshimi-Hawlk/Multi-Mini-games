#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "raylib.h"
#include "types.h"
#include "utils/config.h"


extern Rectangle windowRect;
extern Font      appFont;
extern Color     appBackgroundColor;
extern Font      fonts[MAX_FONT_COUNT];

extern bool  isPlayerTurn;
extern bool  playerHasMoves;
extern bool  computerHasMoves;
extern bool  gameEnded;

extern u64_t playerScore;
extern u64_t computerScore;


extern const Vector2 boardOffset;
extern const Vector3 BOARD_CENTER;


extern Camera3D  camera;
extern Camera3D  savedFreeCamera;
extern bool      isTopDownView;
extern bool      cursorVisible;

extern f32_t     cameraYaw;
extern f32_t     cameraPitch;

extern const f32_t MOVE_SPEED;
extern const f32_t RUN_MULTIPLIER;

extern const Vector3 TOPDOWN_POS;
extern const Vector3 TOPDOWN_TARGET;

extern AnimPiece_St animPieces[BOARD_SIZE * BOARD_SIZE];
extern u64_t animCount;
extern bool animationsPlaying;

extern s64Vector2_St hoveredCell;

extern Model pieceModel;
extern Model boardModel;

extern Model ringModel;

extern Shader lightingShader;
extern Vector3 sunPos;
extern int lightCount;

extern int locViewPos;
extern int locShininess;
extern int locAmbient;
extern int locSpecular;

extern int locLightsEnabled[MAX_LIGHTS];
extern int locLightsType[MAX_LIGHTS];
extern int locLightsPos[MAX_LIGHTS];
extern int locLightsTarget[MAX_LIGHTS];
extern int locLightsColor[MAX_LIGHTS];

#endif // UTILS_GLOBALS_H