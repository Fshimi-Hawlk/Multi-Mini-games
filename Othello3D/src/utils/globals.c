#include "utils/globals.h"
#include "utils/config.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[MAX_FONT_COUNT] = {0};

bool  isPlayerTurn     = true;
bool  playerHasMoves   = true;
bool  computerHasMoves = true;
bool  gameEnded        = false;

u64_t playerScore  = 0;
u64_t computerScore = 0;


const Vector2 boardOffset = {CELL_PX_SIZE, CELL_PX_SIZE};

const Vector3 BOARD_CENTER = {
    BOARD_PX_SIZE / 2.0f, 
    0.0f, 
    BOARD_PX_SIZE / 2.0f 
};


Camera3D camera = {0};
Camera3D savedFreeCamera = {0};
bool     isTopDownView = false;
bool     cursorVisible = false;

float    cameraYaw   = 0.0f;
float    cameraPitch = 0.0f;

const float MOVE_SPEED     = 120.0f;
const float RUN_MULTIPLIER = 3.0f;

const Vector3 TOPDOWN_POS = {BOARD_CENTER.x, 500.0f, BOARD_CENTER.z};
const Vector3 TOPDOWN_TARGET = {BOARD_CENTER.x, 0.0f, BOARD_CENTER.z };

AnimPiece_St animPieces[BOARD_SIZE * BOARD_SIZE] = {0};
u64_t animCount = 0;
bool animationsPlaying = false;

s64Vector2_St hoveredCell = {-1,-1};

Model pieceModel = {0};
Model boardModel = {0};
Model ringModel = {0};

Shader lightingShader = {0};
Vector3 sunPos = {-500.0f, 600.0f, -500.0f};
int lightCount = 0;

int locViewPos = 0;
int locShininess = 0;
int locAmbient = 0;
int locSpecular = 0;

int locLightsEnabled[MAX_LIGHTS] = {0};
int locLightsType[MAX_LIGHTS] = {0};
int locLightsPos[MAX_LIGHTS] = {0};
int locLightsTarget[MAX_LIGHTS] = {0};
int locLightsColor[MAX_LIGHTS] = {0};