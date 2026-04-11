#ifndef CORE_PHYSICS_H
#define CORE_PHYSICS_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define GRAVITY          9.81f
#define FRICTION_FAIRWAY 0.92f
#define FRICTION_ROUGH   0.80f
#define FRICTION_GREEN   0.96f
#define FRICTION_SAND    0.65f
#define BOUNCE_FACTOR    0.35f
#define SLOPE_INFLUENCE  0.4f
#define STOP_THRESHOLD   0.04f

#define BALL_RADIUS_CM     2.134f
#define HOLE_RADIUS_CM     5.4f
#define SCALE  0.1f
#define BALL_R  (BALL_RADIUS_CM  * SCALE)
#define HOLE_R  (HOLE_RADIUS_CM  * SCALE)

typedef struct {
    Vector3     pos;
    Vector3     vel;
    int         state;
    int         surface;
    float       spin;
    float       rot_angle;
    int         strokes;
    int         penalty;
    Vector3     last_valid;
} Ball;

typedef struct {
    float   speed_kmh;
    float   direction_deg;
    Vector3 vec;
} Wind;

typedef enum {
    BALL_IDLE = 0,
    BALL_FLYING,
    BALL_ROLLING,
    BALL_IN_HOLE
} BallState;

typedef enum {
    SURF_FAIRWAY = 0,
    SURF_ROUGH,
    SURF_GREEN,
    SURF_SAND,
    SURF_WATER,
    SURF_OOB
} SurfaceType;

void Ball_Init(Ball *b, Vector3 pos);
void Ball_Shoot(void *g, int club, float aim_angle, float power, Wind *wind);
void Ball_Update(void *g, float dt, float (*getTerrainHeight)(void*, float, float), int (*getSurface)(void*, float, float), Vector3 (*getTerrainNormal)(void*, float, float), Vector3 *shot_positions, int *shot_pos_count);
void Ball_ApplyWindEffect(void *g, float dt, Wind *wind);
bool Ball_IsInHole(void *g, Vector3 hole_pos);

#endif /* CORE_PHYSICS_H */