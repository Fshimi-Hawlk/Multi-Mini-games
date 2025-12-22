#include "setups/game.h"
#include "core/game/board.h"
#include "raylib.h"
#include "utils/globals.h"
#include "utils/logger/logger.h"
#include "utils/utils.h"

void initGame(Board_t board) {
    initBoard(board);

    // Zero everything first
    camera = (Camera3D) {0};

    // Fixed starting free cam view - angled, safe, consistent
    camera.position = TOPDOWN_POS;
    camera.target   = TOPDOWN_TARGET;
    camera.up       = (Vector3) { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    cameraYaw = 180.0f * DEG2RAD;
    cameraPitch = -90.0f * DEG2RAD;

    savedFreeCamera = camera;

    DisableCursor();
    cursorVisible = false;
    isTopDownView = false;

    lightingShader = LoadShader("assets/shaders/lighting.vs", "assets/shaders/lighting.fs");

    locViewPos = GetShaderLocation(lightingShader, "viewPos");
    locShininess = GetShaderLocation(lightingShader, "material.shininess");
    locAmbient = GetShaderLocation(lightingShader, "ambient");

    locLightsEnabled[0] = GetShaderLocation(lightingShader, "lights[0].enabled");
    locLightsType[0] = GetShaderLocation(lightingShader, "lights[0].type");
    locLightsPos[0] = GetShaderLocation(lightingShader, "lights[0].position");
    locLightsTarget[0] = GetShaderLocation(lightingShader, "lights[0].target");
    locLightsColor[0] = GetShaderLocation(lightingShader, "lights[0].color");

    float shininess = 8.0f;
    SetShaderValue(lightingShader, locShininess, &shininess, SHADER_UNIFORM_FLOAT);

    float specularStrength = 0.3f;
    locSpecular = GetShaderLocation(lightingShader, "material.specular");
    if (locSpecular != -1) SetShaderValue(lightingShader, locSpecular, &specularStrength, SHADER_UNIFORM_FLOAT);

    float ambient[4] = {0.05f, 0.05f, 0.05f, 1.0f};
    SetShaderValue(lightingShader, locAmbient, ambient, SHADER_UNIFORM_VEC4);

    int enabled = 1;
    int type = LIGHT_DIRECTIONAL;
    Vector3 lightTarget = {BOARD_CENTER.x, 0.0f, BOARD_CENTER.z};
    float lightColor[4] = {1.5f, 1.5f, 1.4f, 1.0f};

    SetShaderValue(lightingShader, locLightsEnabled[0], &enabled, SHADER_UNIFORM_INT);
    SetShaderValue(lightingShader, locLightsType[0], &type, SHADER_UNIFORM_INT);
    SetShaderValue(lightingShader, locLightsPos[0], &sunPos, SHADER_UNIFORM_VEC3);
    SetShaderValue(lightingShader, locLightsTarget[0], &lightTarget, SHADER_UNIFORM_VEC3);
    SetShaderValue(lightingShader, locLightsColor[0], lightColor, SHADER_UNIFORM_VEC4);

    SetTraceLogLevel(LOG_ERROR);
    pieceModel = LoadModel("assets/models/OthelloHalfPiece.obj");
    pieceModel.materials[0].shader = lightingShader;

    boardModel = LoadModel("assets/models/OthelloBoard.obj");
    boardModel.materials[0].shader = lightingShader;

    SetTraceLogLevel(LOG_WARNING);

    float ringHeight = 2.0f;
    float ringRadius = CELL_PX_SIZE * 0.42f;

    Mesh ringMesh = GenMeshCylinder(ringRadius, ringHeight, 16);
    ringModel = LoadModelFromMesh(ringMesh);
    ringModel.materials[0].shader = lightingShader;
}