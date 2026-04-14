/**
    @file physics.c
    @author Maxime CHAUVEAU
    @date 2026-04-14
    @date 2026-04-14
    @brief Physics simulation implementation for Suika game.
*/
#include "core/physics.h"
#include "core/game.h"
#include "utils/audio.h"
#include "raymath.h"
#include <math.h>
#include <stdlib.h>

/**
    @brief Spawns particles when two fruits merge.

    @param[in,out] game     Pointer to the game state
    @param[in]     position Position where to spawn particles
    @param[in]     color    Color of the particles
*/
static void suika_spawnMergeParticles(SuikaGame_St* game, Vector2 position, Color color)
{
    int count = 8 + (rand() % 5);
    for (int i = 0; i < count && game->particleCount < SUIKA_MAX_PARTICLES; i++)
    {
        Particle_St* p = &game->particles[game->particleCount++];
        float angle = (float)(rand() % 360) * DEG2RAD;
        float speed = 50.0f + (float)(rand() % 100);
        
        p->position = position;
        p->velocity = (Vector2){cosf(angle) * speed, sinf(angle) * speed - 50.0f};
        p->color = color;
        p->color.a = 255;
        p->life = 0.4f + (float)(rand() % 10) / 20.0f;
        p->maxLife = p->life;
        p->size = 3.0f + (float)(rand() % 5);
        p->isActive = true;
    }
}

/**
    @brief Updates the physics simulation for one frame.

    @param[in,out] game      Pointer to the game state
    @param[in]     deltaTime Time elapsed since last frame
*/
void suika_updatePhysics(SuikaGame_St* game, float deltaTime)
{
    (void)deltaTime;
    
    const float FIXED_DT = 0.016f;
    const int COLLISION_PASSES = 8;
    const float GRAVITY = game->gravity;
    const float DAMPING = 0.999f;
    const float MAX_VELOCITY = 2000.0f;
    const float WALL_BOUNCE = 0.3f;
    const float FRICTION = 0.95f;

    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging) continue;
        Fruit_St* f = &game->fruits[i];
        f->velocity.x      *= DAMPING;
        f->velocity.y      *= DAMPING;
        f->angularVelocity *= 0.99f;
    }

    for (int pass = 0; pass < COLLISION_PASSES; pass++)
    {
        for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
        {
            if (!game->fruits[i].isActive || game->fruits[i].isMerging)
                continue;

            Fruit_St* f = &game->fruits[i];
            
            f->velocity.y += GRAVITY * FIXED_DT / (float)COLLISION_PASSES;
            
            if (f->velocity.y > MAX_VELOCITY) f->velocity.y = MAX_VELOCITY;
            if (f->velocity.y < -MAX_VELOCITY) f->velocity.y = -MAX_VELOCITY;
            if (f->velocity.x > MAX_VELOCITY) f->velocity.x = MAX_VELOCITY;
            if (f->velocity.x < -MAX_VELOCITY) f->velocity.x = -MAX_VELOCITY;

            f->position.x += f->velocity.x * FIXED_DT / (float)COLLISION_PASSES;
            f->position.y += f->velocity.y * FIXED_DT / (float)COLLISION_PASSES;
            f->rotation += f->angularVelocity * FIXED_DT / (float)COLLISION_PASSES;
            
            float minX = SUIKA_CONTAINER_X + f->radius;
            float maxX = SUIKA_CONTAINER_X + SUIKA_CONTAINER_WIDTH - f->radius;
            float maxY = SUIKA_CONTAINER_Y + SUIKA_CONTAINER_HEIGHT - f->radius;

            if (f->position.x < minX)
            {
                f->position.x = minX;
                f->velocity.x = -f->velocity.x * WALL_BOUNCE;
            }
            else if (f->position.x > maxX)
            {
                f->position.x = maxX;
                f->velocity.x = -f->velocity.x * WALL_BOUNCE;
            }
            if (f->position.y > maxY)
            {
                f->position.y = maxY;
                f->velocity.y = -f->velocity.y * WALL_BOUNCE;
                if (pass == 0)
                {
                    f->velocity.x *= FRICTION;
                    f->angularVelocity = f->velocity.x / f->radius;
                }

                if (fabsf(f->velocity.y) < 10.0f)
                    f->velocity.y = 0;
            }
        }
        
        for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
        {
            if (!game->fruits[i].isActive || game->fruits[i].isMerging)
                continue;

            Fruit_St* a = &game->fruits[i];

            for (int j = i + 1; j < SUIKA_MAX_FRUITS; j++)
            {
                if (!game->fruits[j].isActive || game->fruits[j].isMerging)
                    continue;

                Fruit_St* b = &game->fruits[j];

                float dx = b->position.x - a->position.x;
                float dy = b->position.y - a->position.y;
                float dist2 = dx*dx + dy*dy;
                float minDist = a->radius + b->radius;
                
                if (dist2 < minDist * minDist && dist2 > 0.0001f)
                {
                    float dist = sqrtf(dist2);
                    float overlap = minDist - dist;
                    
                    float nx = dx / dist;
                    float ny = dy / dist;
                    
                    float correction = overlap * 0.5f;
                    a->position.x -= nx * correction;
                    a->position.y -= ny * correction;
                    b->position.x += nx * correction;
                    b->position.y += ny * correction;
                    
                    float dvx = b->velocity.x - a->velocity.x;
                    float dvy = b->velocity.y - a->velocity.y;
                    float velAlongNormal = dvx * nx + dvy * ny;
                    
                    if (velAlongNormal < 0)
                    {
                        float impulse = velAlongNormal * 0.5f;
                        a->velocity.x += impulse * nx;
                        a->velocity.y += impulse * ny;
                        b->velocity.x -= impulse * nx;
                        b->velocity.y -= impulse * ny;
                    }
                    
                    float tx = -ny;
                    float relTangVel = (b->velocity.x - a->velocity.x) * tx +
                                      (b->velocity.y - a->velocity.y) * (-nx);
                    float spinTransfer = relTangVel * 0.05f;
                    a->angularVelocity -= spinTransfer / a->radius;
                    b->angularVelocity += spinTransfer / b->radius;
                }
            }
        }
    }
}

/**
    @brief Checks for collisions between fruits of the same type and merges them.

    @param[in,out] game Pointer to the game state
*/
void suika_checkMerging(SuikaGame_St* game)
{
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (!game->fruits[i].isActive || game->fruits[i].isMerging)
            continue;

        for (int j = i + 1; j < SUIKA_MAX_FRUITS; j++)
        {
            if (!game->fruits[j].isActive || game->fruits[j].isMerging)
                continue;

            Fruit_St* f1 = &game->fruits[i];
            Fruit_St* f2 = &game->fruits[j];

            if (f1->type == f2->type)
            {
                float dist = Vector2Distance(f1->position, f2->position);
                float touchDist = f1->radius + f2->radius;

                if (dist < touchDist * 1.1f)
                {
                    if (f1->type < FRUIT_WATERMELON)
                    {
                        FruitType_Et newType = (FruitType_Et)(f1->type + 1);
                        const FruitProperties_St* props = suika_getFruitProperties(newType);

                        f1->isMerging = true;
                        f2->isMerging = true;

                        bool slotFound = false;
                        for (int k = 0; k < SUIKA_MAX_FRUITS; k++)
                        {
                            if (!game->fruits[k].isActive)
                            {
                                Vector2 midPos = Vector2Scale(Vector2Add(f1->position, f2->position), 0.5f);

                                game->fruits[k].position = midPos;
                                game->fruits[k].velocity = (Vector2){0.0f, 0.0f};
                                game->fruits[k].type = newType;
                                game->fruits[k].radius = props->radius;
                                game->fruits[k].rotation = (f1->rotation + f2->rotation) * 0.5f;
                                game->fruits[k].angularVelocity = (f1->angularVelocity + f2->angularVelocity) * 0.5f;
                                game->fruits[k].isActive = true;
                                game->fruits[k].isMerging = false;
                                game->fruits[k].id = game->nextFruitId++;

                                if (game->scoreMultiplierEnabled)
                                {
                                    game->score += props->points;
                                }

                                suika_spawnMergeParticles(game, midPos, props->color);

                                PlaySound(sound_merge);

                                f1->isActive = false;
                                f2->isActive = false;

                                slotFound = true;
                                return;
                            }
                        }

                        if (!slotFound)
                        {
                            f1->isMerging = false;
                            f2->isMerging = false;
                        }
                    }
                }
            }
        }
    }
}

/**
    @brief Checks if any fruit has crossed the drop line for too long, triggering game over.

    @param[in,out] game Pointer to the game state
*/
void suika_checkGameOver(SuikaGame_St* game)
{
    for (int i = 0; i < SUIKA_MAX_FRUITS; i++)
    {
        if (game->fruits[i].isActive)
        {
            if (game->fruits[i].position.y - game->fruits[i].radius < SUIKA_DROP_LINE_Y)
            {
                if (fabsf(game->fruits[i].velocity.y) < 10.0f)
                {
                    game->isGameOver = true;
                    if (game->score > game->highScore)
                    {
                        game->highScore = game->score;
                    }
                    game->base.running = false;
                }
            }
        }
    }
}
