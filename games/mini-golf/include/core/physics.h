/**
    @file physics.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Physics simulation for the golf ball.
*/
#ifndef CORE_PHYSICS_H
#define CORE_PHYSICS_H

#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define GRAVITY          9.81f  ///< Earth gravity acceleration (m/s^2)
#define FRICTION_FAIRWAY 0.92f  ///< Friction coefficient for fairway grass
#define FRICTION_ROUGH   0.80f  ///< Friction coefficient for rough grass
#define FRICTION_GREEN   0.96f  ///< Friction coefficient for putting green
#define FRICTION_SAND    0.65f  ///< Friction coefficient for bunkers
#define BOUNCE_FACTOR    0.35f  ///< Energy conservation after bounce
#define SLOPE_INFLUENCE  0.4f   ///< Gravity effect on rolling ball
#define STOP_THRESHOLD   0.04f  ///< Minimum speed before ball stops

#define BALL_RADIUS_CM     2.134f  ///< Official golf ball radius in cm
#define HOLE_RADIUS_CM     5.4f    ///< Official golf hole radius in cm
#define SCALE  0.1f                ///< World scale factor (1 unit = 10m)
#define BALL_R  (BALL_RADIUS_CM  * SCALE) ///< Scaled ball radius
#define HOLE_R  (HOLE_RADIUS_CM  * SCALE) ///< Scaled hole radius

/* Note: Ball, Wind, BallState, SurfaceType are also defined in game.h.
   Ideally they should be in a shared types header. */

/**
    @brief Initialized a ball at a given position.

    @param[out] b    Pointer to the ball to initialize.
    @param[in]  pos  Initial position.
    @return          void
*/
void Ball_Init(Ball *b, Vector3 pos);

/**
    @brief Simulates a shot based on club selection and power.

    @param[in,out] g          Pointer to the GolfGame state.
    @param[in]     club       Index of the selected club.
    @param[in]     aim_angle  Horizontal angle of the shot.
    @param[in]     power      Power of the shot (0.0 to 1.0).
    @param[in]     wind       Current wind conditions.
    @return                   void
*/
void Ball_Shoot(void *g, int club, float aim_angle, float power, Wind *wind);

/**
    @brief Updates ball physics for a single frame.

    @param[in,out] g                  Pointer to the GolfGame state.
    @param[in]     dt                 Delta time since last frame.
    @param[in]     getTerrainHeight   Callback to get terrain height at (x, z).
    @param[in]     getSurface         Callback to get surface type at (x, z).
    @param[in]     getTerrainNormal   Callback to get terrain normal at (x, z).
    @param[out]    shot_positions     Array to store ball trajectory.
    @param[in,out] shot_pos_count     Pointer to the count of stored trajectory positions.
    @return                           void
*/
void Ball_Update(void *g, float dt, float (*getTerrainHeight)(void*, float, float), int (*getSurface)(void*, float, float), Vector3 (*getTerrainNormal)(void*, float, float), Vector3 *shot_positions, int *shot_pos_count);

/**
    @brief Applies wind force to the ball during flight.

    @param[in,out] g     Pointer to the GolfGame state.
    @param[in]     dt    Delta time.
    @param[in]     wind  Current wind conditions.
    @return              void
*/
void Ball_ApplyWindEffect(void *g, float dt, Wind *wind);

/**
    @brief Checks if the ball has entered the hole.

    @param[in,out] g         Pointer to the GolfGame state.
    @param[in]     hole_pos  Position of the hole.
    @return                  True if the ball is in the hole.
*/
bool Ball_IsInHole(void *g, Vector3 hole_pos);

#endif /* CORE_PHYSICS_H */

