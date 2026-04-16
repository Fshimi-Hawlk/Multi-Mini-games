/**
    @file golf_camera.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief 3D Camera management for Golf 3D.
*/
#include "golf.h"

#define CAM_PITCH_MIN  5.0f   ///< Minimum camera pitch in degrees
#define CAM_PITCH_MAX  89.0f  ///< Maximum camera pitch in degrees
#define CAM_DIST_MIN   1.5f   ///< Minimum camera distance from target
#define CAM_DIST_MAX   80.0f  ///< Maximum camera distance from target
#define CAM_SMOOTH     0.08f  ///< Camera smoothing factor

/**
    @brief Initializes the golf camera.
    @param[out] c      Camera to initialize.
    @param[in]  target Initial target position.
*/
void GCam_Init(GolfCamera *c, Vector3 target) {
    float yrad, prad;

    c->mode     = CAM_ORBIT;
    c->yaw      = 180.0f;
    c->pitch    = 35.0f;
    c->distance = 12.0f;
    c->target   = target;
    c->fov      = 60.0f;

    c->cam.projection = CAMERA_PERSPECTIVE;
    c->cam.up         = (Vector3){0,1,0};
    c->cam.fovy       = c->fov;
    c->cam.target     = target;

    yrad = c->yaw   * DEG2RAD;
    prad = c->pitch * DEG2RAD;
    c->cam.position = (Vector3){
        target.x + c->distance * sinf(yrad) * cosf(prad),
        target.y + c->distance * sinf(prad),
        target.z + c->distance * cosf(yrad) * cosf(prad),
    };
}

/**
    @brief Updates the camera position and orientation based on current mode.
    @param[in,out] c  Camera to update.
    @param[in]     g  Game state.
    @param[in]     dt Delta time.
*/
void GCam_Update(GolfCamera *c, GolfGame *g, float dt) {
    Ball    *b              = &g->ball;
    Vector3  desired_target = b->pos;
    float    yrad, prad;
    Vector3  offset, desired_pos;

    switch (c->mode) {

    case CAM_ORBIT:
        if (g->state == STATE_AIMING || g->state == STATE_POWER) {
            Vector2 md    = GetMouseDelta();
            float   wheel = GetMouseWheelMove();
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                c->yaw   -= md.x * 0.3f;
                c->pitch  = Clamp(c->pitch + md.y * 0.3f,
                                  CAM_PITCH_MIN, CAM_PITCH_MAX);
            }
            c->distance = Clamp(c->distance - wheel, CAM_DIST_MIN, CAM_DIST_MAX);

            if (g->state == STATE_AIMING) {
                float target_yaw = g->aim_angle + 180.0f;
                float diff       = target_yaw - c->yaw;
                while (diff >  180.0f) diff -= 360.0f;
                while (diff < -180.0f) diff += 360.0f;
                c->yaw += diff * 0.05f;
            }
        }
        break;

    case CAM_FOLLOW:
        if (b->state == BALL_FLYING || b->state == BALL_ROLLING) {
            float   spd = Vector3Length(b->vel);
            if (spd > 0.1f) {
                Vector3 back       = Vector3Normalize(Vector3Negate(b->vel));
                float   target_yaw = -(atan2f(back.x, back.z) * RAD2DEG);
                float   diff       = target_yaw - c->yaw;
                while (diff >  180.0f) diff -= 360.0f;
                while (diff < -180.0f) diff += 360.0f;
                c->yaw      += diff * 0.04f;
                c->pitch     = Clamp(15.0f + spd*0.5f, 15.0f, 45.0f);
                c->distance  = 8.0f + spd*0.3f;
            }
        }
        break;

    case CAM_TOP:
        c->pitch    = 89.0f;
        c->distance = 50.0f;
        break;

    case CAM_HOLE:
        {
            HoleData *h = &g->holes[g->current_hole];
            desired_target = (Vector3){
                (h->tee_pos.x + h->hole_pos.x) * 0.5f,
                0.0f,
                (h->tee_pos.z + h->hole_pos.z) * 0.5f
            };
            c->pitch    = 70.0f;
            c->distance = h->distance_m * SCALE * 0.8f;
        }
        break;
    }

    c->target = Vector3Lerp(c->target, desired_target, CAM_SMOOTH + dt);

    yrad = c->yaw   * DEG2RAD;
    prad = c->pitch * DEG2RAD;
    offset = (Vector3){
        c->distance * sinf(yrad) * cosf(prad),
        c->distance * sinf(prad),
        c->distance * cosf(yrad) * cosf(prad),
    };
    desired_pos = Vector3Add(c->target, offset);

    c->cam.position = Vector3Lerp(c->cam.position, desired_pos,
                                  (c->mode == CAM_FOLLOW) ? 0.12f : 0.10f);
    c->cam.target = c->target;
    c->cam.fovy   = c->fov;

    if (IsKeyPressed(KEY_F1)) GCam_SetMode(c, CAM_ORBIT);
    if (IsKeyPressed(KEY_F2)) GCam_SetMode(c, CAM_FOLLOW);
    if (IsKeyPressed(KEY_F3)) GCam_SetMode(c, CAM_TOP);
    if (IsKeyPressed(KEY_F4)) GCam_SetMode(c, CAM_HOLE);
}

/**
    @brief Changes the camera mode and sets default distance.
    @param[in,out] c    Camera state.
    @param[in]     mode New camera mode.
*/
void GCam_SetMode(GolfCamera *c, CamMode mode) {
    c->mode = mode;
    if (mode == CAM_FOLLOW) c->distance = 8.0f;
    if (mode == CAM_ORBIT)  c->distance = 12.0f;
}

/**
    @brief Starts 3D rendering mode for the camera.
    @param[in] c Camera state.
*/
void GCam_BeginMode3D(GolfCamera *c) { BeginMode3D(c->cam); }

/**
    @brief Ends 3D rendering mode.
*/
void GCam_EndMode3D(void)            { EndMode3D(); }

