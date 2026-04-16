/**
    @file physics.h
    @author Maxime CHAUVEAU
    @date 2026-03-01
    @date 2026-04-14
    @brief Physics module for the Bowling mini-game.
*/
#ifndef BOWLING_PHYSICS_H
#define BOWLING_PHYSICS_H

#include <raylib.h>
#include <stdbool.h>

/**
    @brief Maximum number of active particles in the system.
*/
#define MAX_PARTICLES 50

/**
    @brief State of a single bowling pin.
*/
typedef struct {
    Vector3 position;           ///< 3D position in the world
    Vector3 velocity;           ///< Linear velocity vector
    Vector3 angularVelocity;    ///< Angular velocity vector
    Vector3 rotation;           ///< Current rotation (Euler angles or similar)
    bool isStanding;            ///< true if the pin has not been knocked down
    float mass;                 ///< Mass of the pin for collision physics
    float fallTime;             ///< Time since the pin started falling
    float rotationAngle;        ///< Current tilt angle from vertical
    Vector3 rotationAxis;       ///< Axis around which the pin is rotating
} Pin_St;

/**
    @brief State of the bowling ball.
*/
typedef struct {
    Vector3 position;           ///< 3D position in the world
    Vector3 velocity;           ///< Linear velocity vector
    Vector3 spin;               ///< Spin direction vector
    float spinAmount;           ///< Magnitude of the spin effect
    float radius;               ///< Physical radius of the ball
    float mass;                 ///< Mass of the ball for collision physics
    bool isRolling;             ///< true if the ball is currently active on the lane
    float visualRotation;       ///< Cumulative rotation angle in degrees for rendering
    Vector3 rollAxis;           ///< World-space axis around which the ball rolls
} Ball_St;

/**
    @brief State of a single physics particle (e.g., for collisions).
*/
typedef struct {
    Vector3 position;           ///< 3D position in the world
    Vector3 velocity;           ///< Linear velocity vector
    Color color;                ///< Color of the particle
    float life;                 ///< Remaining life time in seconds
    float maxLife;              ///< Initial life time in seconds
    float size;                 ///< Visual size of the particle
} Particle_St;

/**
    @brief Initializes the particle system.

    @param[out] particles       Array of particles to initialize
    @param[out] particleCount   Pointer to the active particle count (set to 0)
*/
void physics_initParticles(Particle_St* particles, int* particleCount);

/**
    @brief Spawns a group of particles at a given position.

    @param[in,out] particles       Particle array
    @param[in,out] particleCount   Pointer to the current particle count
    @param[in]     position        Spawn center position
    @param[in]     count           Number of particles to spawn
    @param[in]     baseColor       Initial color of the particles
*/
void physics_spawnParticles(Particle_St* particles, int* particleCount, Vector3 position, int count, Color baseColor);

/**
    @brief Updates all active particles.

    @param[in,out] particles       Particle array
    @param[in,out] particleCount   Pointer to the current particle count
    @param[in]     deltaTime       Time elapsed since last frame
*/
void physics_updateParticles(Particle_St* particles, int* particleCount, float deltaTime);

/**
    @brief Sets up the initial positions and states of the pins.

    @param[out] pins    Array of 10 pins to initialize
*/
void physics_setupPins(Pin_St* pins);

/**
    @brief Resets the ball to its starting position and state.

    @param[in,out] ball    Ball handle to reset
*/
void physics_resetBall(Ball_St* ball);

/**
    @brief Launches the ball with a given direction, power, and spin.

    @param[in,out] ball        Ball handle
    @param[in]     direction   Launch direction vector (should be normalized)
    @param[in]     power       Launch speed multiplier
    @param[in]     spinAmount  Initial spin factor
*/
void physics_launchBall(Ball_St* ball, Vector3 direction, float power, float spinAmount);

/**
    @brief Updates the ball's velocity based on its current spin.

    @param[in,out] ball        Ball handle
    @param[in]     deltaTime   Time elapsed since last frame
*/
void physics_updateBallSpin(Ball_St* ball, float deltaTime);

/**
    @brief Checks and resolves collisions between ball, pins, and particles.

    @param[in,out] ball            Ball handle
    @param[in,out] pins            Array of pins
    @param[in,out] particles       Particle system
    @param[in,out] particleCount   Active particle count
*/
void physics_checkCollisions(Ball_St* ball, Pin_St* pins, Particle_St* particles, int* particleCount);

/**
    @brief Checks if the ball is currently in the gutters.

    @param[in]     ball        Ball handle
    @param[in]     laneWidth   Width of the bowling lane
    @param[in]     gutterWidth Width of the gutters
    @return                    true if the ball is in a gutter
*/
bool physics_isGutterBall(Ball_St* ball, float laneWidth, float gutterWidth);

/**
    @brief Updates the ball's position and physics.

    @param[in,out] ball        Ball handle
    @param[in]     deltaTime   Time elapsed since last frame
    @param[in]     laneWidth   Width of the bowling lane
    @param[in]     gutterWidth Width of the gutters
    @param[in]     bumpers     true if bumpers are active
*/
void physics_updateBall(Ball_St* ball, float deltaTime, float laneWidth, float gutterWidth, bool bumpers);

/**
    @brief Updates the positions and states of all pins.

    @param[in,out] pins        Array of pins
    @param[in]     pinCount    Number of pins in the array
    @param[in]     deltaTime   Time elapsed since last frame
    @param[in]     laneWidth   Width of the bowling lane
    @param[in]     laneLength  Length of the bowling lane
*/
void physics_updatePins(Pin_St* pins, int pinCount, float deltaTime, float laneWidth, float laneLength);

/**
    @brief Checks if the ball has reached the pin deck area.

    @param[in]     ball    Ball handle
    @return                true if the ball reached the pins
*/
bool physics_hasBallReachedPins(Ball_St* ball);

#endif // BOWLING_PHYSICS_H
