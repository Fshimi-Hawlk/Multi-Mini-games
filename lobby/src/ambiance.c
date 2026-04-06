#include "config.h"
#include "constant.h"
#include "global.h"

#include "ambiance.h"

static Firefly_St fireflies[MAX_FIREFLIES] = {0};

// Dynamic wind gust system
static float windGustStrength = 0.0f;

void initAtmosphericEffects(void) {
    // nothing to do
}

void updateAtmosphericEffects(float dt, Player_st* player, Camera2D cam) {
    // Compute visible world rectangle + padding so nothing pops in/out of nowhere
    float padding = 180.0f;
    Rectangle view = {
        cam.target.x - WINDOW_WIDTH / 2.0f - padding,
        cam.target.y - WINDOW_HEIGHT / 2.0f - padding,
        WINDOW_WIDTH + padding * 2.0f,
        WINDOW_HEIGHT + padding * 2.0f
    };

    // ── Fireflies ───────────────────────────────────────────────────────────
    bool isInitialBurst = (gameTime < 3.0f);

    static float fireflySpawnTimer = 0.0f;
    
    if (!isInitialBurst) {
        fireflySpawnTimer -= dt;
    }

    // Initial burst at game start + gradual respawn for recycled ones
    if (isInitialBurst || fireflySpawnTimer <= 0.0f) {
        // fireflySpawnTimer = 0.22f + (rand() % 18) / 100.0f;

        for (int i = 0; i < MAX_FIREFLIES; ++i) {
            if (fireflies[i].active) continue;

            // At very beginning (first ~3 seconds) spawn inside camera + padding
            // After that, only recycle in the outer padding ring so it never feels empty
            float spawnPadding = 280.0f;

            Rectangle spawnArea = {
                cam.target.x - WINDOW_WIDTH / 2.0f - spawnPadding,
                cam.target.y - WINDOW_HEIGHT / 2.0f - spawnPadding * 0.6f,
                WINDOW_WIDTH + spawnPadding * 2.0f,
                WINDOW_HEIGHT + spawnPadding * 1.2f
            };

            fireflies[i].position.x = spawnArea.x + (rand() % (int)spawnArea.width);
            fireflies[i].position.y = spawnArea.y + (rand() % (int)spawnArea.height);

            // Post-initial-burst: force recycled fireflies into the outer padding ring
            // (between camera bounds and despawn distance) - no more inside-camera spawns
            if (!isInitialBurst) {
                Rectangle innerCamera = {
                    cam.target.x - WINDOW_WIDTH / 2.0f + 40.0f,
                    cam.target.y - WINDOW_HEIGHT / 2.0f + 40.0f,
                    WINDOW_WIDTH - 80.0f,
                    WINDOW_HEIGHT - 80.0f
                };
                int tries = 0;
                while (CheckCollisionPointRec(fireflies[i].position, innerCamera) && tries < 8) {
                    fireflies[i].position.x = spawnArea.x + (rand() % (int)spawnArea.width);
                    fireflies[i].position.y = spawnArea.y + (rand() % (int)spawnArea.height);
                    tries++;
                }
            }

            fireflies[i].velocity = (Vector2){(rand() % 30 - 15) * 0.5f, (rand() % 25 - 12) * 0.5f};
            fireflies[i].radius = 2.0f + (rand() % 3);
            fireflies[i].alpha = 0.4f;
            fireflies[i].phase = (rand() % 628) / 100.0f;
            fireflies[i].active = true;

            // Mode probabilities: Wander 55%, Bob 30%, Loop 15%
            int roll = rand() % 100;
            if (roll < 55) {
                fireflies[i].mode = FIREFLY_MODE_WANDER;
            } else if (roll < 85) {
                fireflies[i].mode = FIREFLY_MODE_BOB;
            } else {
                fireflies[i].mode = FIREFLY_MODE_LOOP;
            }

            fireflies[i].modeTimer = 4.5f + (rand() % 950) / 100.0f;   // 4.5-14 s before switching
            fireflies[i].facingAngle = (float)(rand() % 360) * DEG2RAD;

            if (fireflies[i].mode == FIREFLY_MODE_WANDER) {
                fireflies[i].wanderTarget = fireflies[i].position;   // force immediate new target
            } else if (fireflies[i].mode == FIREFLY_MODE_LOOP) {
                fireflies[i].loopCount = 5 + (rand() % 8);           // 5-12 waypoints
                for (int j = 0; j < fireflies[i].loopCount; ++j) {
                    float a = (float)(rand() % 360) * DEG2RAD;
                    float dist = 95.0f + (rand() % 135);             // middle-to-long range between waypoints
                    fireflies[i].loopPoints[j] = Vector2Add(
                        fireflies[i].position,
                        (Vector2){cosf(a) * dist, sinf(a) * dist}
                    );
                }
                fireflies[i].currentLoopIndex = 0;
            }

            break;   // only spawn/respawn one per timer tick
        }
    }

    for (int i = 0; i < MAX_FIREFLIES; ++i) {
        Firefly_St* f = &fireflies[i];
        if (!f->active) continue;

        f->phase += dt * 4.2f;

        // Mode timer & switching (with new probabilities on switch)
        f->modeTimer -= dt;
        if (f->modeTimer <= 0.0f) {
            int roll = rand() % 100;
            if (roll < 55) {
                f->mode = FIREFLY_MODE_WANDER;
            } else if (roll < 85) {
                f->mode = FIREFLY_MODE_BOB;
            } else {
                f->mode = FIREFLY_MODE_LOOP;
            }

            f->modeTimer = 4.5f + (rand() % 950) / 100.0f;

            if (f->mode == FIREFLY_MODE_LOOP) {
                f->loopCount = 5 + (rand() % 8);
                for (int j = 0; j < f->loopCount; ++j) {
                    float a = (float)(rand() % 360) * DEG2RAD;
                    float dist = 95.0f + (rand() % 135);
                    f->loopPoints[j] = Vector2Add(f->position, (Vector2){cosf(a) * dist, sinf(a) * dist});
                }
                f->currentLoopIndex = rand() % f->loopCount;
            } else if (f->mode == FIREFLY_MODE_WANDER) {
                f->wanderTarget = f->position;   // force immediate new target
            }
        }

        // ── Determine current movement target ───────────────────────────────
        Vector2 target = f->position;

        if (f->mode == FIREFLY_MODE_WANDER) {
            if (Vector2Distance(f->position, f->wanderTarget) < 38.0f) {
                // New wander target: ±65° cone around facingAngle, 15% full random direction
                float angle = f->facingAngle;
                if ((rand() % 100) < 15) {
                    angle = (float)(rand() % 360) * DEG2RAD;   // full 360° change
                } else {
                    angle += ((rand() % 131) - 65) * DEG2RAD;  // ±65°
                }
                float dist = 105.0f + (rand() % 155);          // middle-to-long range (approx 105-260 units)
                Vector2 dir = (Vector2){cosf(angle), sinf(angle)};
                f->wanderTarget = Vector2Add(f->position, Vector2Scale(dir, dist));
                f->facingAngle = angle;
            }
            target = f->wanderTarget;
        } 
        else if (f->mode == FIREFLY_MODE_LOOP && f->loopCount > 0) {
            target = f->loopPoints[f->currentLoopIndex];
            if (Vector2Distance(f->position, target) < 32.0f) {
                f->currentLoopIndex = (f->currentLoopIndex + 1) % f->loopCount;
            }
        }
        // BOB mode has no real target (just bob in place)

        // ── Movement towards target with random speed variation ─────────────
        Vector2 toTarget = Vector2Subtract(target, f->position);
        float distToTarget = Vector2Length(toTarget);

        if ((f->mode == FIREFLY_MODE_WANDER || f->mode == FIREFLY_MODE_LOOP) && distToTarget > 6.0f) {
            Vector2 desiredDir = Vector2Normalize(toTarget);
            
            // Random speed: slow (28) to speedy (82)
            float currentSpeed = 28.0f + (rand() % 55);
            Vector2 desiredVel = Vector2Scale(desiredDir, currentSpeed);

            f->velocity = Vector2Add(f->velocity, Vector2Scale(Vector2Subtract(desiredVel, f->velocity), 7.2f * dt));
        }

        // ── Strong player flee (kept from previous improvement) ─────────────
        Vector2 toPlayer = Vector2Subtract(player->position, f->position);
        float distSq = Vector2LengthSqr(toPlayer);
        if (distSq < 32000.0f && distSq > 0.001f) {
            Vector2 fleeDir = Vector2Normalize(Vector2Negate(toPlayer));
            float strength = (32000.0f - distSq) / 32000.0f * 680.0f;
            f->velocity = Vector2Add(f->velocity, Vector2Scale(fleeDir, strength * dt));
        }

        // Always-present wander/bob force
        Vector2 wanderForce = {
            cosf(f->phase) * 28.0f,
            sinf(f->phase * 1.4f) * 18.0f - 6.0f
        };
        f->velocity = Vector2Add(f->velocity, Vector2Scale(wanderForce, 0.82f * dt));

        // Apply velocity + damping + wind
        f->position = Vector2Add(f->position, Vector2Scale(f->velocity, dt));
        f->velocity = Vector2Scale(f->velocity, 0.87f);
        f->velocity.x += windGustStrength * 38.0f * dt;

        // Vertical bob (stronger in BOB mode)
        float bobStrength = (f->mode == FIREFLY_MODE_BOB) ? 15.0f : 8.5f;
        f->position.y += sinf(f->phase * 3.1f) * bobStrength * dt;

        // Fade out & recycle when leaving view (soft outer bound)
        if (!CheckCollisionPointRec(f->position, view)) {
            f->alpha -= dt * 3.1f;
            if (f->alpha <= 0.0f) {
                f->active = false;
            }
        } else {
            f->alpha = 0.38f + sinf(f->phase * 1.8f) * 0.48f;
        }
    }
}

void drawAtmosphericEffects(void) {
    // Fireflies - mode aware color for debugging
    for (int i = 0; i < MAX_FIREFLIES; ++i) {
        Firefly_St* f = &fireflies[i];
        if (!f->active) continue;

        Color baseGlow;
        switch (f->mode) {
            case FIREFLY_MODE_WANDER: {
                baseGlow = (Color){255, 235, 110, 255};  // warm yellow
            } break;

            case FIREFLY_MODE_LOOP: {
                baseGlow = (Color){120, 255, 160, 255};  // green
            } break;

            case FIREFLY_MODE_BOB: {
                baseGlow = (Color){255, 120, 120, 255};  // soft red
            } break;

            default: {
                baseGlow = YELLOW;
            }
        }

        Color glow = Fade(baseGlow, f->alpha);
        DrawCircleV(f->position, f->radius + 2.5f, Fade(glow, 0.26f));
        DrawCircleV(f->position, f->radius, glow);
    }
}