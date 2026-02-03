/**
 * @file bowling.h
 * @brief Main header for Bowling mini-game
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#ifndef BOWLING_BOWLING_H
#define BOWLING_BOWLING_H

#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdint.h>

// Game constants
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define TARGET_FPS 60

#define NUM_PINS 10
#define MAX_FRAMES 10
#define MAX_ROLLS_PER_FRAME 2

// Pin positions (standard arrangement)
typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 rotation;
    bool isStanding;
    float mass;
} Pin_St;

// Bowling ball
typedef struct {
    Vector3 position;
    Vector3 velocity;
    float radius;
    float mass;
    bool isRolling;
} Ball_St;

// Score frame
typedef struct {
    int rolls[3];      // Max 3 rolls in 10th frame
    int numRolls;
    int score;
    bool isStrike;
    bool isSpare;
} Frame_St;

// Game state
typedef struct {
    Ball_St ball;
    Pin_St pins[NUM_PINS];
    Frame_St frames[MAX_FRAMES];
    int currentFrame;
    int totalScore;
    Camera3D camera;
    bool isAiming;
    Vector2 aimStart;
    Vector2 aimCurrent;
    float power;
} BowlingGame_St;

// Function declarations

/** @brief Initialize bowling game */
void bowling_init(BowlingGame_St* game);

/** @brief Update game logic */
void bowling_update(BowlingGame_St* game, float deltaTime);

/** @brief Draw game */
void bowling_draw(const BowlingGame_St* game);

/** @brief Cleanup resources */
void bowling_cleanup(BowlingGame_St* game);

/** @brief Setup pins in standard formation */
void bowling_setupPins(Pin_St pins[NUM_PINS]);

/** @brief Launch ball with given direction and power */
void bowling_launchBall(Ball_St* ball, Vector3 direction, float power);

/** @brief Update physics for all objects */
void bowling_updatePhysics(BowlingGame_St* game, float deltaTime);

/** @brief Check collisions between ball and pins */
void bowling_checkCollisions(BowlingGame_St* game);

/** @brief Calculate score for current frame */
void bowling_calculateScore(BowlingGame_St* game);

/** @brief Draw HUD with scores */
void bowling_drawHUD(const BowlingGame_St* game);

#endif // BOWLING_BOWLING_H
