/**
 * @file physics.h
 * @author Maxime CHAUVEAU
 * @date March 2026
 * @brief Physics module for the Bowling mini-game.
 *
 * Contains all physics-related functions:
 *   - ball physics, spin, and aiming
 *   - pin collisions with tumble physics
 *   - particle system physics
 */

#ifndef BOWLING_PHYSICS_H
#define BOWLING_PHYSICS_H

#include <raylib.h>
#include <stdbool.h>

#define MAX_PARTICLES 50

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 angularVelocity;
    Vector3 rotation;
    bool isStanding;
    float mass;
    float fallTime;
    float rotationAngle;
    Vector3 rotationAxis;
} Pin_St;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 spin;
    float spinAmount;
    float radius;
    float mass;
    bool isRolling;
    float visualRotation;  ///< Cumulative rotation angle in degrees
    Vector3 rollAxis;      ///< World-space axis around which the ball rolls (perpendicular to velocity)
} Ball_St;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Color color;
    float life;
    float maxLife;
    float size;
} Particle_St;

void physics_initParticles(Particle_St* particles, int* particleCount);
void physics_spawnParticles(Particle_St* particles, int* particleCount, Vector3 position, int count, Color baseColor);
void physics_updateParticles(Particle_St* particles, int* particleCount, float deltaTime);
void physics_setupPins(Pin_St* pins);
void physics_resetBall(Ball_St* ball);
void physics_launchBall(Ball_St* ball, Vector3 direction, float power, float spinAmount);
void physics_updateBallSpin(Ball_St* ball, float deltaTime);
void physics_checkCollisions(Ball_St* ball, Pin_St* pins, Particle_St* particles, int* particleCount);
bool physics_isGutterBall(Ball_St* ball, float laneWidth, float gutterWidth);
void physics_updateBall(Ball_St* ball, float deltaTime, float laneWidth, float gutterWidth, bool bumpers);
void physics_updatePins(Pin_St* pins, int pinCount, float deltaTime, float laneWidth, float laneLength);

#endif
