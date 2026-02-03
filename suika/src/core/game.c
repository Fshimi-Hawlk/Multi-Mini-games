/**
 * @file game.c
 * @brief Core Suika game logic
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#include "suika.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Fruit properties (radius, color, points, emoji)
static const FruitProperties_St FRUIT_PROPS[FRUIT_TYPE_COUNT] = {
    {20.0f,  {255, 0, 0, 255},     10,   "🍒"},  // Cherry
    {25.0f,  {255, 100, 100, 255}, 20,   "🍓"},  // Strawberry
    {30.0f,  {128, 0, 128, 255},   30,   "🍇"},  // Grape
    {35.0f,  {255, 165, 0, 255},   40,   "🍊"},  // Orange
    {40.0f,  {255, 140, 0, 255},   50,   "🟠"},  // Persimmon
    {45.0f,  {255, 0, 0, 255},     60,   "🍎"},  // Apple
    {50.0f,  {173, 255, 47, 255},  70,   "🍐"},  // Pear
    {55.0f,  {255, 218, 185, 255}, 80,   "🍑"},  // Peach
    {60.0f,  {255, 215, 0, 255},   90,   "🍍"},  // Pineapple
    {70.0f,  {144, 238, 144, 255}, 100,  "🍈"},  // Melon
    {80.0f,  {34, 139, 34, 255},   150,  "🍉"}   // Watermelon
};

const FruitProperties_St* suika_getFruitProperties(FruitType_Et type) {
    if (type >= 0 && type < FRUIT_TYPE_COUNT) {
        return &FRUIT_PROPS[type];
    }
    return &FRUIT_PROPS[0];
}

void suika_init(SuikaGame_St* game) {
    memset(game, 0, sizeof(SuikaGame_St));
    
    // Initialize game state
    game->nextFruitId = 0;
    game->nextFruitX = CONTAINER_X + CONTAINER_WIDTH / 2.0f;
    game->canDrop = true;
    game->score = 0;
    game->highScore = 0;
    game->isGameOver = false;
    game->gravity = 800.0f;
    
    // Initialize all fruits as inactive
    for (int i = 0; i < MAX_FRUITS; i++) {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
    }
    
    // Spawn first fruit
    suika_spawnNextFruit(game);
}

void suika_spawnNextFruit(SuikaGame_St* game) {
    // Random fruit type (only small fruits at start)
    FruitType_Et type = (FruitType_Et)(rand() % 5); // Cherry to Persimmon
    const FruitProperties_St* props = suika_getFruitProperties(type);
    
    game->nextFruit.type = type;
    game->nextFruit.radius = props->radius;
    game->nextFruit.position = (Vector2){game->nextFruitX, DROP_LINE_Y - props->radius};
    game->nextFruit.velocity = (Vector2){0.0f, 0.0f};
    game->nextFruit.isActive = false;
    game->nextFruit.isMerging = false;
    game->nextFruit.id = game->nextFruitId++;
}

void suika_dropFruit(SuikaGame_St* game) {
    if (!game->canDrop || game->isGameOver) return;
    
    // Find empty slot
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (!game->fruits[i].isActive) {
            game->fruits[i] = game->nextFruit;
            game->fruits[i].isActive = true;
            game->fruits[i].position.x = game->nextFruitX;
            
            game->canDrop = false;
            
            // Spawn next fruit after a delay
            suika_spawnNextFruit(game);
            
            break;
        }
    }
}

void suika_update(SuikaGame_St* game, float deltaTime) {
    if (game->isGameOver) {
        game->gameOverTimer += deltaTime;
        
        if (IsKeyPressed(KEY_R)) {
            suika_reset(game);
        }
        return;
    }
    
    // Move next fruit with mouse
    Vector2 mousePos = GetMousePosition();
    float minX = CONTAINER_X + game->nextFruit.radius;
    float maxX = CONTAINER_X + CONTAINER_WIDTH - game->nextFruit.radius;
    game->nextFruitX = Clamp(mousePos.x, minX, maxX);
    game->nextFruit.position.x = game->nextFruitX;
    
    // Drop fruit on click
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        suika_dropFruit(game);
    }
    
    // Update physics
    suika_updatePhysics(game, deltaTime);
    
    // Check merging
    suika_checkMerging(game);
    
    // Check game over
    suika_checkGameOver(game);
    
    // Allow next drop after fruits settle
    static float dropTimer = 0.0f;
    if (!game->canDrop) {
        dropTimer += deltaTime;
        if (dropTimer > 1.0f) {
            game->canDrop = true;
            dropTimer = 0.0f;
        }
    }
}

void suika_updatePhysics(SuikaGame_St* game, float deltaTime) {
    // Update each fruit
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging) continue;
        
        Fruit_St* fruit = &game->fruits[i];
        
        // Apply gravity
        fruit->velocity.y += game->gravity * deltaTime;
        
        // Update position
        fruit->position.x += fruit->velocity.x * deltaTime;
        fruit->position.y += fruit->velocity.y * deltaTime;
        
        // Container boundaries
        float minX = CONTAINER_X + fruit->radius;
        float maxX = CONTAINER_X + CONTAINER_WIDTH - fruit->radius;
        float maxY = CONTAINER_Y + CONTAINER_HEIGHT - fruit->radius;
        
        // Left/right walls
        if (fruit->position.x < minX) {
            fruit->position.x = minX;
            fruit->velocity.x = -fruit->velocity.x * 0.5f;
        } else if (fruit->position.x > maxX) {
            fruit->position.x = maxX;
            fruit->velocity.x = -fruit->velocity.x * 0.5f;
        }
        
        // Bottom
        if (fruit->position.y > maxY) {
            fruit->position.y = maxY;
            fruit->velocity.y = 0.0f;
            fruit->velocity.x *= 0.9f; // Friction
        }
        
        // Fruit-fruit collisions
        for (int j = i + 1; j < MAX_FRUITS; j++) {
            if (!game->fruits[j].isActive || game->fruits[j].isMerging) continue;
            
            Fruit_St* other = &game->fruits[j];
            float dist = Vector2Distance(fruit->position, other->position);
            float minDist = fruit->radius + other->radius;
            
            if (dist < minDist && dist > 0.0f) {
                // Collision response
                Vector2 normal = Vector2Normalize(Vector2Subtract(other->position, fruit->position));
                float overlap = minDist - dist;
                
                fruit->position = Vector2Subtract(fruit->position, Vector2Scale(normal, overlap * 0.5f));
                other->position = Vector2Add(other->position, Vector2Scale(normal, overlap * 0.5f));
                
                // Velocity reflection (simplified)
                Vector2 relativeVel = Vector2Subtract(fruit->velocity, other->velocity);
                float velAlongNormal = Vector2DotProduct(relativeVel, normal);
                
                if (velAlongNormal < 0) {
                    Vector2 impulse = Vector2Scale(normal, velAlongNormal * 0.8f);
                    fruit->velocity = Vector2Subtract(fruit->velocity, impulse);
                    other->velocity = Vector2Add(other->velocity, impulse);
                }
            }
        }
    }
}

void suika_checkMerging(SuikaGame_St* game) {
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging) continue;
        
        for (int j = i + 1; j < MAX_FRUITS; j++) {
            if (!game->fruits[j].isActive || game->fruits[j].isMerging) continue;
            
            Fruit_St* f1 = &game->fruits[i];
            Fruit_St* f2 = &game->fruits[j];
            
            // Check if same type and touching
            if (f1->type == f2->type) {
                float dist = Vector2Distance(f1->position, f2->position);
                float touchDist = f1->radius + f2->radius;
                
                if (dist < touchDist * 1.1f) {
                    // Can merge (if not watermelon)
                    if (f1->type < FRUIT_WATERMELON) {
                        // Create new merged fruit
                        FruitType_Et newType = (FruitType_Et)(f1->type + 1);
                        const FruitProperties_St* props = suika_getFruitProperties(newType);
                        
                        // Find empty slot
                        for (int k = 0; k < MAX_FRUITS; k++) {
                            if (!game->fruits[k].isActive) {
                                Vector2 midPos = Vector2Scale(Vector2Add(f1->position, f2->position), 0.5f);
                                
                                game->fruits[k].position = midPos;
                                game->fruits[k].velocity = (Vector2){0.0f, 0.0f};
                                game->fruits[k].type = newType;
                                game->fruits[k].radius = props->radius;
                                game->fruits[k].isActive = true;
                                game->fruits[k].isMerging = false;
                                game->fruits[k].id = game->nextFruitId++;
                                
                                // Add score
                                game->score += props->points;
                                
                                // Remove old fruits
                                f1->isActive = false;
                                f2->isActive = false;
                                
                                return; // Only one merge per frame
                            }
                        }
                    }
                }
            }
        }
    }
}

void suika_checkGameOver(SuikaGame_St* game) {
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (game->fruits[i].isActive) {
            if (game->fruits[i].position.y - game->fruits[i].radius < DROP_LINE_Y) {
                // Check if fruit has settled (not just dropped)
                if (fabsf(game->fruits[i].velocity.y) < 10.0f) {
                    game->isGameOver = true;
                    if (game->score > game->highScore) {
                        game->highScore = game->score;
                    }
                }
            }
        }
    }
}

void suika_reset(SuikaGame_St* game) {
    game->score = 0;
    game->isGameOver = false;
    game->gameOverTimer = 0.0f;
    game->canDrop = true;
    game->nextFruitId = 0;
    
    for (int i = 0; i < MAX_FRUITS; i++) {
        game->fruits[i].isActive = false;
        game->fruits[i].isMerging = false;
    }
    
    suika_spawnNextFruit(game);
}

void suika_draw(const SuikaGame_St* game) {
    ClearBackground(RAYWHITE);
    
    // Draw container
    DrawRectangleLines(CONTAINER_X, CONTAINER_Y, CONTAINER_WIDTH, CONTAINER_HEIGHT, BLACK);
    
    // Draw drop line
    DrawLine(CONTAINER_X, DROP_LINE_Y, CONTAINER_X + CONTAINER_WIDTH, DROP_LINE_Y, RED);
    
    // Draw fruits
    for (int i = 0; i < MAX_FRUITS; i++) {
        if (game->fruits[i].isActive) {
            const FruitProperties_St* props = suika_getFruitProperties(game->fruits[i].type);
            DrawCircleV(game->fruits[i].position, game->fruits[i].radius, props->color);
            DrawCircleLines((int)game->fruits[i].position.x, (int)game->fruits[i].position.y, 
                          game->fruits[i].radius, BLACK);
        }
    }
    
    // Draw next fruit (if can drop)
    if (game->canDrop && !game->isGameOver) {
        const FruitProperties_St* props = suika_getFruitProperties(game->nextFruit.type);
        DrawCircleV(game->nextFruit.position, game->nextFruit.radius, 
                   Fade(props->color, 0.5f));
        DrawCircleLines((int)game->nextFruit.position.x, (int)game->nextFruit.position.y,
                       game->nextFruit.radius, GRAY);
    }
    
    // Draw HUD
    suika_drawHUD(game);
}

void suika_drawHUD(const SuikaGame_St* game) {
    DrawText("SUIKA GAME", 10, 10, 30, BLACK);
    DrawText(TextFormat("Score: %d", game->score), 10, 50, 20, DARKBLUE);
    DrawText(TextFormat("Best: %d", game->highScore), 10, 75, 20, DARKGREEN);
    
    if (game->isGameOver) {
        DrawRectangle(0, SCREEN_HEIGHT/2 - 50, SCREEN_WIDTH, 100, Fade(BLACK, 0.7f));
        DrawText("GAME OVER!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 20, 40, RED);
        DrawText("Press R to restart", SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 30, 20, WHITE);
    } else {
        DrawText("Click to drop fruit", 10, SCREEN_HEIGHT - 60, 16, DARKGRAY);
        DrawText("R: Restart | ESC: Quit", 10, SCREEN_HEIGHT - 35, 16, DARKGRAY);
    }
}

void suika_cleanup(SuikaGame_St* game) {
    (void)game;
}
