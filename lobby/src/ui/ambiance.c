/**
    @file ambiance.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief ambiance.c implementation/header file
*/
#include "utils/globals.h"

#include "ui/ambiance.h"

static Firefly_St fireflies[MAX_FIREFLIES] = {0};
static FallingLeaf_St fallingLeaves[MAX_FALLING_LEAVES] = {0};

// Dynamic wind gust system
static float windGustStrength = 0.0f;

/**
    @brief Returns a random point inside the tree canopy annular sector.
    Uses uniform angle + sqrt-radius for even visual coverage across the whole crown.
    Small jitter prevents any visual clustering.
*/
static Vector2 getRandomCanopySpawnPoint(void) {
    float t = (float)(rand() % 10000) / 10000.0f;
    float angleDeg = CANOPY_ARC_START_ANGLE + t * (CANOPY_ARC_END_ANGLE - CANOPY_ARC_START_ANGLE);
    float angleRad = angleDeg * DEG2RAD;

    // sqrt(t) gives better area distribution (more leaves in the middle of the crown)
    float radius = CANOPY_RADIUS_MIN + sqrtf(t) * (CANOPY_RADIUS_MAX - CANOPY_RADIUS_MIN);

    Vector2 pos = {
        CANOPY_CENTER_X + cosf(angleRad) * radius,
        CANOPY_CENTER_Y + sinf(angleRad) * radius
    };

    // tiny jitter to break any visual clustering
    pos.x += -22.0f + (float)(rand() % 45);
    pos.y += -15.0f + (float)(rand() % 31);

    return pos;
}

/**
    @brief Applies a gentle push to a leaf (airborne or grounded).
    Activates temporary strong rotational drag for the requested 3-5 s window.
*/
static void pushLeafByPlayer(FallingLeaf_St* leaf, const Player_St* player) {
    Vector2 dir = Vector2Subtract(leaf->position, player->position);
    float distSq = Vector2LengthSqr(dir);

    if (distSq < 0.001f || distSq > 4800.0f) {
        return;
    }

    dir = Vector2Normalize(dir);
    float strength = (4800.0f - distSq) / 4800.0f * LEAF_PLAYER_PUSH;

    leaf->velocity = Vector2Add(leaf->velocity, Vector2Scale(dir, strength));

    // controlled additive spin (much calmer than before)
    float spinAmount = ((rand() % 2) ? 1.0f : -1.0f) * (1.35f + (float)(rand() % 110) / 100.0f);
    leaf->rotationSpeed += spinAmount;

    // activate temporary strong drag (exactly what you asked for)
    leaf->spinDampTimer = LEAF_SPIN_DAMP_TIME + ((float)(rand() % 8) / 10.0f);   // 4.3-5.1 s

    leaf->life += 3.8f;
    if (leaf->life > LEAF_BASE_LIFE * 1.65f) {
        leaf->life = LEAF_BASE_LIFE * 1.65f;
    }
}

/**
    @brief Checks for landing on the *top surface only* of any platform.
    Side or bottom contacts are ignored. Landing is now softer (no hard snap).
*/
static bool leafLandedOnPlatformTop(FallingLeaf_St* leaf) {
    float leafRadius = 11.0f * leaf->scale;

    da_foreach(LobbyTerrain_St, t, &terrains) {
        Rectangle r = t->rect;

        if (CheckCollisionCircleRec(leaf->position, leafRadius, r)) {
            // Only accept as "top landing" if coming from above and close to the top edge
            if (leaf->velocity.y > 0.0f && leaf->position.y < r.y + 18.0f) {
                // Soft landing - only correct if actually penetrating
                float penetration = (r.y - leafRadius * 0.6f) - leaf->position.y;
                if (penetration < 0.0f) {
                    leaf->position.y += penetration * 0.6f;   // gentle correction, no hard snap
                }
                leaf->velocity.y = 0.0f;
                leaf->velocity.x *= 0.55f;   // some friction
                return true;
            }
            // Side hit - just bounce horizontally, stay airborne
            else {
                leaf->velocity.x += (leaf->position.x < r.x + r.width * 0.5f) ? -65.0f : 65.0f;
            }
        }
    }
    return false;
}

void lobby_updateAtmosphericEffects(float dt, Player_St* player, Camera2D cam) {
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

    // ── Falling Leaves ──────────────────────────────────────────────────────
    static float leafSpawnTimer = 0.0f;
    leafSpawnTimer -= dt;

    // Continuous gentle spawning only – no initial burst
    if (leafSpawnTimer <= 0.0f) {
        leafSpawnTimer = 1.25f + (float)(rand() % 60) / 100.0f;   // ~1.25-1.85 s

        for (int i = 0; i < MAX_FALLING_LEAVES; ++i) {
            if (fallingLeaves[i].active) continue;

            FallingLeaf_St* l = &fallingLeaves[i];
            l->position = getRandomCanopySpawnPoint();
            l->velocity = (Vector2){
                -22.0f + (float)(rand() % 62),
                9.0f + (float)(rand() % 34)
            };
            l->rotation = (float)(rand() % 360) * DEG2RAD;
            l->rotationSpeed = -1.9f + (float)(rand() % 380) / 100.0f;
            l->scale = 0.75f + (float)(rand() % 45) / 100.0f;
            l->life = LEAF_BASE_LIFE * 0.98f;
            l->currentAlpha = 1.0f;
            l->active = true;
            l->onGround = false;
            l->groundTimer = 0.0f;
            l->spinDampTimer = 0.0f;

            int g = 75 + (rand() % 55);
            l->color = (Color){48 + (rand() % 35), g, 38 + (rand() % 30), 255};
            break;
        }
    }

    // Update every leaf – completely camera-independent
    for (int i = 0; i < MAX_FALLING_LEAVES; ++i) {
        FallingLeaf_St* l = &fallingLeaves[i];
        if (!l->active) continue;

        l->life -= dt;

        if (!l->onGround) {
            // ── Airborne physics (pure world-space, constant speed) ────────
            l->velocity.y += LEAF_GRAVITY * dt;

            // Gentle long-term drift / curving (still present even if flutter = 0)
            l->velocity.x += sinf(gameTime * LEAF_DRIFT_FREQUENCY + (float)i * 2.3f) *
                             LEAF_DRIFT_AMPLITUDE * dt;

            // Fast visible flutter / sway on top
            l->velocity.x += sinf(gameTime * LEAF_FLUTTER_FREQUENCY + (float)i) *
                             LEAF_FLUTTER_AMPLITUDE * dt;

            // Wind + air drag
            l->velocity.x += windGustStrength * 52.0f * dt;
            l->velocity = Vector2Scale(l->velocity, 0.968f);

            l->position = Vector2Add(l->position, Vector2Scale(l->velocity, dt));
            l->rotation += l->rotationSpeed * dt;

            // Rotation drag (temporary strong drag only after player push)
            if (l->spinDampTimer > 0.0f) {
                l->rotationSpeed *= LEAF_ROT_DRAG_STRONG;
                l->spinDampTimer -= dt;
            } else {
                l->rotationSpeed *= LEAF_ROT_DRAG_NORMAL;
            }

            // Player interaction in air
            if (CheckCollisionCircles(l->position, 9.2f * l->scale,
                                      player->position, player->radius + 5.0f)) {
                pushLeafByPlayer(l, player);
            }

            // Intelligent platform top landing (soft)
            if (leafLandedOnPlatformTop(l)) {
                l->onGround = true;
                l->groundTimer = LEAF_GROUND_TIME + (float)(rand() % 650) / 100.0f;
                l->rotationSpeed *= 0.35f;
            }
            // Fallback absolute ground
            else if (l->position.y > GROUND_Y + 30.0f) {
                l->position.y = GROUND_Y;
                l->velocity = Vector2Zero();
                l->onGround = true;
                l->groundTimer = LEAF_GROUND_TIME + (float)(rand() % 650) / 100.0f;
            }
        }
        else {
            // ── On ground / platform ───────────────────────────────────────
            l->groundTimer -= dt;
            l->velocity = Vector2Zero();

            // Player can knock it off the platform
            if (CheckCollisionCircles(l->position, 9.2f * l->scale,
                                      player->position, player->radius + 5.0f)) {
                pushLeafByPlayer(l, player);
                l->onGround = false;
                l->groundTimer = 0.0f;
            }

            if (l->groundTimer <= 0.0f) {
                l->currentAlpha -= dt * 0.92f;
                if (l->currentAlpha <= 0.0f) {
                    l->active = false;
                }
            }
        }

        // Graceful airborne fade-out when life expires (no popping)
        if (l->life <= 0.0f && !l->onGround) {
            l->currentAlpha -= dt * 1.15f;
            if (l->currentAlpha <= 0.0f) {
                l->active = false;
            }
        }
    }
}

void lobby_drawAtmosphericEffects(void) {
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

    // Falling leaves (unchanged except active filter)
    for (int i = 0; i < MAX_FALLING_LEAVES; ++i) {
        FallingLeaf_St* l = &fallingLeaves[i];
        if (!l->active) continue;

        float finalAlpha = l->onGround ? l->currentAlpha : 1.0f;

        DrawRectanglePro(
            (Rectangle){l->position.x, l->position.y, 18.0f * l->scale, 5.0f * l->scale},
            (Vector2){9.0f * l->scale, 2.5f * l->scale},
            l->rotation * RAD2DEG,
            Fade(l->color, finalAlpha)
        );

        DrawLineEx(l->position,
                   (Vector2){l->position.x + sinf(l->rotation) * 9.0f * l->scale,
                             l->position.y - cosf(l->rotation) * 2.5f * l->scale},
                   1.1f, Fade(WHITE, 0.18f * finalAlpha));
    }
}

void lobby_drawScreenEffects(Player_St* player) {
    float heightFactor = 1 - Clamp((GROUND_Y - player->position.y) / 650.0f, 0.0f, 1.0f); // stronger near ground

    // Vignette + night grading - much softer when player is high
    DrawRectangleGradientV(0, 0, systemSettings.video.width, systemSettings.video.height,
                           Fade(BLACK, 0.0f),
                           Fade(PURPLE, 0.19f * heightFactor));

    DrawRectangle(0, 0, systemSettings.video.width, systemSettings.video.height, Fade(BLACK, 0.09f));
}