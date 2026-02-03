/**
 * @file game.c
 * @brief Core bowling game logic
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "bowling.h"
#include <math.h>
#include <string.h>

// Pin arrangement in triangular formation
static const Vector3 PIN_POSITIONS[NUM_PINS] = {
    {0.0f, 0.0f, -15.0f},      // Pin 1 (front)
    {-0.5f, 0.0f, -16.0f},     // Pin 2
    {0.5f, 0.0f, -16.0f},      // Pin 3
    {-1.0f, 0.0f, -17.0f},     // Pin 4
    {0.0f, 0.0f, -17.0f},      // Pin 5
    {1.0f, 0.0f, -17.0f},      // Pin 6
    {-1.5f, 0.0f, -18.0f},     // Pin 7
    {-0.5f, 0.0f, -18.0f},     // Pin 8
    {0.5f, 0.0f, -18.0f},      // Pin 9
    {1.5f, 0.0f, -18.0f}       // Pin 10
};

void bowling_init(BowlingGame_St* game) {
    memset(game, 0, sizeof(BowlingGame_St));
    
    // Setup camera
    game->camera.position = (Vector3){0.0f, 3.0f, 8.0f};
    game->camera.target = (Vector3){0.0f, 0.0f, -10.0f};
    game->camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    game->camera.fovy = 45.0f;
    game->camera.projection = CAMERA_PERSPECTIVE;
    
    // Setup ball
    game->ball.position = (Vector3){0.0f, 0.3f, 0.0f};
    game->ball.velocity = (Vector3){0.0f, 0.0f, 0.0f};
    game->ball.radius = 0.3f;
    game->ball.mass = 7.0f;
    game->ball.isRolling = false;
    
    // Setup pins
    bowling_setupPins(game->pins);
    
    // Initialize frames
    for (int i = 0; i < MAX_FRAMES; i++) {
        game->frames[i].numRolls = 0;
        game->frames[i].score = 0;
        game->frames[i].isStrike = false;
        game->frames[i].isSpare = false;
    }
    
    game->currentFrame = 0;
    game->totalScore = 0;
    game->isAiming = false;
}

void bowling_setupPins(Pin_St pins[NUM_PINS]) {
    for (int i = 0; i < NUM_PINS; i++) {
        pins[i].position = PIN_POSITIONS[i];
        pins[i].position.y = 0.5f; // Height off ground
        pins[i].velocity = (Vector3){0.0f, 0.0f, 0.0f};
        pins[i].rotation = (Vector3){0.0f, 0.0f, 0.0f};
        pins[i].isStanding = true;
        pins[i].mass = 1.5f;
    }
}

void bowling_update(BowlingGame_St* game, float deltaTime) {
    // Handle aiming
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->ball.isRolling) {
        game->isAiming = true;
        game->aimStart = GetMousePosition();
    }
    
    if (game->isAiming) {
        game->aimCurrent = GetMousePosition();
        
        Vector2 diff = Vector2Subtract(game->aimCurrent, game->aimStart);
        game->power = fminf(Vector2Length(diff) / 100.0f, 3.0f);
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            game->isAiming = false;
            
            // Launch ball
            Vector3 direction = {-diff.x / 100.0f, 0.0f, -1.0f};
            direction = Vector3Normalize(direction);
            bowling_launchBall(&game->ball, direction, game->power * 10.0f);
        }
    }
    
    // Update physics
    if (game->ball.isRolling) {
        bowling_updatePhysics(game, deltaTime);
        bowling_checkCollisions(game);
    }
    
    // Check if ball stopped
    if (game->ball.isRolling && Vector3Length(game->ball.velocity) < 0.1f) {
        game->ball.isRolling = false;
        
        // Count knocked pins
        int knockedPins = 0;
        for (int i = 0; i < NUM_PINS; i++) {
            if (!game->pins[i].isStanding) {
                knockedPins++;
            }
        }
        
        // Update score
        Frame_St* frame = &game->frames[game->currentFrame];
        frame->rolls[frame->numRolls++] = knockedPins;
        
        bowling_calculateScore(game);
    }
    
    // Reset ball on space
    if (IsKeyPressed(KEY_SPACE)) {
        game->ball.position = (Vector3){0.0f, 0.3f, 0.0f};
        game->ball.velocity = (Vector3){0.0f, 0.0f, 0.0f};
        game->ball.isRolling = false;
        bowling_setupPins(game->pins);
    }
}

void bowling_launchBall(Ball_St* ball, Vector3 direction, float power) {
    ball->velocity = Vector3Scale(direction, power);
    ball->isRolling = true;
}

void bowling_updatePhysics(BowlingGame_St* game, float deltaTime) {
    // Update ball position
    Vector3 acceleration = Vector3Scale(game->ball.velocity, -0.5f); // Friction
    game->ball.velocity = Vector3Add(game->ball.velocity, Vector3Scale(acceleration, deltaTime));
    game->ball.position = Vector3Add(game->ball.position, Vector3Scale(game->ball.velocity, deltaTime));
    
    // Keep ball on lane
    if (game->ball.position.y < 0.3f) {
        game->ball.position.y = 0.3f;
    }
    
    // Update pins
    for (int i = 0; i < NUM_PINS; i++) {
        if (!game->pins[i].isStanding) {
            // Apply gravity and friction to fallen pins
            game->pins[i].velocity.y -= 9.8f * deltaTime;
            game->pins[i].velocity = Vector3Scale(game->pins[i].velocity, 0.95f);
            game->pins[i].position = Vector3Add(game->pins[i].position, 
                                                Vector3Scale(game->pins[i].velocity, deltaTime));
            
            if (game->pins[i].position.y < 0.0f) {
                game->pins[i].position.y = 0.0f;
                game->pins[i].velocity.y = 0.0f;
            }
        }
    }
}

void bowling_checkCollisions(BowlingGame_St* game) {
    // Ball vs pins
    for (int i = 0; i < NUM_PINS; i++) {
        if (game->pins[i].isStanding) {
            float dist = Vector3Distance(game->ball.position, game->pins[i].position);
            float minDist = game->ball.radius + 0.15f; // Pin radius approximation
            
            if (dist < minDist) {
                // Collision detected
                game->pins[i].isStanding = false;
                
                // Transfer momentum
                Vector3 normal = Vector3Normalize(Vector3Subtract(game->pins[i].position, 
                                                                  game->ball.position));
                float impulseMagnitude = 2.0f * Vector3DotProduct(game->ball.velocity, normal);
                game->pins[i].velocity = Vector3Scale(normal, impulseMagnitude);
            }
        }
    }
    
    // Pin vs pin collisions
    for (int i = 0; i < NUM_PINS; i++) {
        for (int j = i + 1; j < NUM_PINS; j++) {
            if (!game->pins[i].isStanding && !game->pins[j].isStanding) continue;
            
            float dist = Vector3Distance(game->pins[i].position, game->pins[j].position);
            if (dist < 0.3f) { // Two pin radii
                if (game->pins[i].isStanding) game->pins[i].isStanding = false;
                if (game->pins[j].isStanding) game->pins[j].isStanding = false;
            }
        }
    }
}

void bowling_calculateScore(BowlingGame_St* game) {
    // Simplified scoring - count knocked pins
    int totalKnocked = 0;
    for (int i = 0; i < NUM_PINS; i++) {
        if (!game->pins[i].isStanding) {
            totalKnocked++;
        }
    }
    
    Frame_St* frame = &game->frames[game->currentFrame];
    
    if (frame->numRolls == 1 && totalKnocked == NUM_PINS) {
        frame->isStrike = true;
        frame->score = 10;
    } else if (frame->numRolls == 2) {
        frame->score = totalKnocked;
        if (totalKnocked == NUM_PINS) {
            frame->isSpare = true;
        }
    }
    
    game->totalScore += frame->score;
}

void bowling_draw(const BowlingGame_St* game) {
    BeginMode3D(game->camera);
    
    // Draw lane
    DrawPlane((Vector3){0.0f, 0.0f, -10.0f}, (Vector2){3.0f, 25.0f}, DARKGRAY);
    
    // Draw ball
    DrawSphere(game->ball.position, game->ball.radius, RED);
    
    // Draw pins
    for (int i = 0; i < NUM_PINS; i++) {
        Color pinColor = game->pins[i].isStanding ? WHITE : GRAY;
        DrawCylinder(game->pins[i].position, 0.15f, 0.1f, 1.0f, 8, pinColor);
    }
    
    // Draw aiming guide
    if (game->isAiming) {
        Vector3 aimDir = {-(game->aimCurrent.x - game->aimStart.x) / 100.0f, 
                          0.0f, -1.0f};
        aimDir = Vector3Normalize(aimDir);
        Vector3 aimEnd = Vector3Add(game->ball.position, Vector3Scale(aimDir, 5.0f));
        DrawLine3D(game->ball.position, aimEnd, YELLOW);
    }
    
    EndMode3D();
    
    // Draw HUD
    bowling_drawHUD(game);
}

void bowling_drawHUD(const BowlingGame_St* game) {
    // Score display
    DrawText(TextFormat("Frame: %d", game->currentFrame + 1), 10, 10, 20, BLACK);
    DrawText(TextFormat("Score: %d", game->totalScore), 10, 35, 20, BLACK);
    
    if (game->isAiming) {
        DrawText(TextFormat("Power: %.1f", game->power), 10, 60, 20, BLUE);
    }
    
    // Instructions
    DrawText("Click & Drag to aim, Release to throw", 10, SCREEN_HEIGHT - 30, 16, DARKGRAY);
    DrawText("SPACE: Reset | ESC: Quit", 10, SCREEN_HEIGHT - 10, 16, DARKGRAY);
}

void bowling_cleanup(BowlingGame_St* game) {
    // Nothing to cleanup for now (no dynamic allocations)
    (void)game;
}
