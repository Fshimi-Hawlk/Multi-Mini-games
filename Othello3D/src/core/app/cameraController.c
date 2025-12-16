#include "core/app/cameraController.h"
#include "raylib.h"
#include "utils/globals.h"
#include "core/game/boardUtils.h"
#include "utils/logger/logger.h"

s64Vector2_St updateCamera(void) {
    s64Vector2_St clickedPos = {-1, -1};

    // Alt toggle for cursor (only in free cam)
    if (IsKeyPressed(KEY_LEFT_ALT)) {
        cursorVisible = !cursorVisible;
        if (cursorVisible) ShowCursor();
        else DisableCursor();
    }

    // Ctrl+A toggle mode
    if (IsKeyPressed(KEY_A) && (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL))) {
        isTopDownView = !isTopDownView;

        if (isTopDownView) {
            savedFreeCamera = camera;
            
            // Set position high above
            camera.position = TOPDOWN_POS;

            // Set a safe pitch (-89°) and yaw to look straight down with tiny offset
            cameraYaw = 180.0f * DEG2RAD;  // or 0, whatever your orientation
            cameraPitch = -90.0f * DEG2RAD;  // safe, not -90

            // Recompute target from yaw/pitch to avoid exact down
            Vector3 forward;
            forward.x = sinf(cameraYaw) * cosf(cameraPitch);
            forward.y = sinf(cameraPitch);
            forward.z = cosf(cameraYaw) * cosf(cameraPitch);
            forward = Vector3Normalize(forward);

            camera.target = Vector3Add(camera.position, Vector3Scale(forward, camera.position.y));  // long distance to board

            camera.up = (Vector3){0.0f, 1.0f, 0.0f};
            camera.fovy = 60.0f;

            ShowCursor();
            cursorVisible = true;
        } else {
            camera = savedFreeCamera;
            DisableCursor();
            cursorVisible = false;
        }
    }

    // Free camera movement and look
    if (!isTopDownView || !cursorVisible) {
        Vector2 mouseDelta = GetMouseDelta();

        // Update yaw and pitch
        cameraYaw   -= mouseDelta.x * CAMERA_SENSITIVITY * GetFrameTime();
        cameraPitch -= mouseDelta.y * CAMERA_SENSITIVITY * GetFrameTime();

        // Clamp pitch
        const float PITCH_LIMIT = 89.0f * DEG2RAD;
        if (cameraPitch > PITCH_LIMIT) cameraPitch = PITCH_LIMIT;
        if (cameraPitch < -PITCH_LIMIT) cameraPitch = -PITCH_LIMIT;

        // Recompute forward
        Vector3 forward;
        forward.x = sinf(cameraYaw) * cosf(cameraPitch);
        forward.y = sinf(cameraPitch);
        forward.z = cosf(cameraYaw) * cosf(cameraPitch);
        forward = Vector3Normalize(forward);

        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0.0f, 1.0f, 0.0f}));

        // Movement
        float speed = MOVE_SPEED * GetFrameTime();
        if (IsKeyDown(KEY_LEFT_SHIFT)) speed *= RUN_MULTIPLIER;

        if (IsKeyDown(KEY_W)) camera.position = Vector3Add(camera.position, Vector3Scale(forward, speed));
        if (IsKeyDown(KEY_S)) camera.position = Vector3Add(camera.position, Vector3Scale(forward, -speed));
        if (IsKeyDown(KEY_A)) camera.position = Vector3Subtract(camera.position, Vector3Scale(right, speed));
        if (IsKeyDown(KEY_D)) camera.position = Vector3Add(camera.position, Vector3Scale(right, speed));

        // Update target
        float lookDistance = 1.0f;
        camera.target = Vector3Add(camera.position, Vector3Scale(forward, lookDistance));
    }

    // Click detection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsCursorOnScreen()) {
        if (cursorVisible || isTopDownView) {
            Vector2 mouse = GetMousePosition();
            clickedPos = getBoardCellFromRay(mouse, camera);
        }
    }

    // Toggle button
    Rectangle toggleBtn = {(float)GetScreenWidth() - 170, (float)GetScreenHeight() - 70, 160, 50 };
    bool buttonClicked = CheckCollisionPointRec(GetMousePosition(), toggleBtn) &&
                         IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                         (cursorVisible || isTopDownView);

    if (buttonClicked) {
        isTopDownView = !isTopDownView;
        if (isTopDownView) {
            savedFreeCamera = camera;
            
            // Set position high above
            camera.position = TOPDOWN_POS;

            // Set a safe pitch (-89°) and yaw to look straight down with tiny offset
            cameraYaw = 180.0f * DEG2RAD;  // or 0, whatever your orientation
            cameraPitch = -90.0f * DEG2RAD;  // safe, not -90

            // Recompute target from yaw/pitch to avoid exact down
            Vector3 forward;
            forward.x = sinf(cameraYaw) * cosf(cameraPitch);
            forward.y = sinf(cameraPitch);
            forward.z = cosf(cameraYaw) * cosf(cameraPitch);
            forward = Vector3Normalize(forward);

            camera.target = Vector3Add(camera.position, Vector3Scale(forward, camera.position.y));  // long distance to board

            camera.up = (Vector3){0.0f, 1.0f, 0.0f};
            camera.fovy = 60.0f;

            ShowCursor();
            cursorVisible = true;
        } else {
            camera = savedFreeCamera;
            DisableCursor();
            cursorVisible = false;
        }
    }

    log_debug("Camera pos: (%.3f, %.3f, %.3f) | Camera target (%.3f, %.3f, %.3f) | yaw: %.3f pitch: %.3f", 
        camera.position.x, camera.position.y, camera.position.z,
        camera.target.x, camera.target.y, camera.target.z,
        cameraYaw * RAD2DEG, cameraPitch * RAD2DEG
    );

    s64Vector2_St hoveredPos = {-1, -1};
    if (cursorVisible && isPlayerTurn) {  // assume isPlayerTurn global
        Vector2 mouse = GetMousePosition();
        hoveredPos = getBoardCellFromRay(mouse, camera);
    }

    // Store hovered for drawing (add new global s64Vector2_St hoveredCell = {-1,-1}; in globals)
    hoveredCell = hoveredPos;

    return clickedPos;
}