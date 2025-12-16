#include "setups/game.h"
#include "core/game/board.h"
#include "raylib.h"
#include "utils/globals.h"

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

    // Reverse compute yaw/pitch from the fixed vectors (safe, no -90°)
    // Vector3 forward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    cameraYaw = 180.0f * DEG2RAD;
    cameraPitch = -90.0f * DEG2RAD;

    savedFreeCamera = camera;

    DisableCursor();
    cursorVisible = false;
    isTopDownView = false;

    log_debug("Startup camera: pos (%.1f, %.1f, %.1f), target (%.1f, %.1f, %.1f), up (%.1f, %.1f, %.1f), yaw %.1f deg, pitch %.1f deg",
        camera.position.x, camera.position.y, camera.position.z,
        camera.target.x, camera.target.y, camera.target.z,
        camera.up.x, camera.up.y, camera.up.z,
        cameraYaw * RAD2DEG, cameraPitch * RAD2DEG
    );
}