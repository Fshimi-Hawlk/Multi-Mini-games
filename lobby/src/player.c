#include "constant.h"
#include "global.h"

#include "audio.h"

#include "player.h"

static void resolveCircleRectCollision(Player_st* player, Rectangle rect) {
    // point le plus proche du cercle sur le rectangle
    float closestX = Clamp(player->position.x, rect.x, rect.x + rect.width);
    float closestY = Clamp(player->position.y, rect.y, rect.y + rect.height);

    float dx = player->position.x - closestX;
    float dy = player->position.y - closestY;

    float distSq = dx*dx + dy*dy;
    float r = player->radius;

    if (distSq >= r * r)
        return;

    float dist = sqrtf(distSq);
    if (dist == 0)
        return;

    float penetration = r - dist;

    float nx = dx / dist;
    float ny = dy / dist;

    // correction position
    player->position.x += nx * penetration;
    player->position.y += ny * penetration;

    // résolution vitesse selon l’axe dominant
    if (fabsf(nx) > fabsf(ny)) {
        player->velocity.x = 0;
    }
    else {
        player->velocity.y = 0;

        // sol
        if (ny < 0) {
            player->onGround = true;
            player->nbJumps = 0;
            player->coyoteTimer = COYOTE_TIME;
        }
    }
}

void drawPlayer(Player_st* player) {
    // ── Moonlight-based offsets ─────────────────────────────────────────────
    Vector2 shadowOffset   = Vector2Scale(moonLightDir, -player->radius * 0.45f);
    Vector2 glowOffset     = Vector2Scale(moonLightDir,  player->radius * 0.38f);

    // ── 1. Soft drop shadow (drawn first, under everything) ────────────────
    Color shadowColor = Fade(BLACK, 0.28f);

    if (player->texture == NULL) {
        DrawCircleV((Vector2){
            player->position.x + shadowOffset.x,
            player->position.y + shadowOffset.y
        }, player->radius, shadowColor);
    } else {
        DrawTexturePro(*player->texture,
            (Rectangle){0, 0, (float)player->texture->width, (float)player->texture->height},
            (Rectangle){
                player->position.x + shadowOffset.x,
                player->position.y + shadowOffset.y,
                player->radius * 2,
                player->radius * 2
            },
            (Vector2){player->radius, player->radius},
            player->angle,
            shadowColor);
    }

    // ── 2. Main player ─────────────────────────────────────────────────────
    if (player->texture == NULL) {
        DrawCircleV(player->position, player->radius, BLUE);
    } else {
        DrawTexturePro(
            *player->texture,
            (Rectangle){0, 0, (float)player->texture->width, (float)player->texture->height},
            (Rectangle){
                player->position.x,
                player->position.y,
                player->radius * 2,
                player->radius * 2
            },
            (Vector2){player->radius, player->radius},
            player->angle,
            WHITE
        );
    }

    // ── 3. Very subtle layered glow (firefly-style) ────────────────────────
    Vector2 glowPos = {
        player->position.x + glowOffset.x,
        player->position.y + glowOffset.y
    };

    Color glowBase = (Color){180, 220, 255, 255};   // soft moonlight cyan-white

    // Outer soft glow layer
    DrawCircleV(glowPos, player->radius * 0.72f, Fade(glowBase, 0.09f));
    // Medium glow layer
    DrawCircleV(glowPos, player->radius * 0.48f, Fade(glowBase, 0.14f));
    // Tight inner glow layer
    DrawCircleV(glowPos, player->radius * 0.26f, Fade(glowBase, 0.11f));
}

void updatePlayer(Player_st* player, Platform_st* platforms, int nbPlatforms, float dt) {
    // INPUT HORIZONTAL
    if (IsKeyDown(KEY_A))
        player->velocity.x = -300;
    else if (IsKeyDown(KEY_D))
        player->velocity.x = 300;
    else {
        // friction quand aucune touche n'est pressée
        if (player->velocity.x > 0) {
            player->velocity.x -= FRICTION * dt;
            if (player->velocity.x < 0) player->velocity.x = 0;
        } else if (player->velocity.x < 0) {
            player->velocity.x += FRICTION * dt;
            if (player->velocity.x > 0) player->velocity.x = 0;
        }
    }

    if (IsKeyPressed(KEY_R)) {
        player->position = (Vector2) {0};
        player->velocity = (Vector2) {0};
    }

    // Rotation en fonction de la direction
    if (player->velocity.x > 0) {
        player->angle += 360 * dt; // tourner dans le sens horaire
    }
    else if (player->velocity.x < 0) {
        player->angle -= 360 * dt; // tourner dans le sens anti-horaire
    }

    // INPUT JUMP -> buffer
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    }
    else if (player->jumpBuffer > 0) {
        player->jumpBuffer -= dt;
        if (player->jumpBuffer < 0)
            player->jumpBuffer = 0;
    }

    // GRAVITÉ
    player->velocity.y += 1200 * dt;

    // COLLISIONS
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    // Empêcher de sortir à gauche
    if (player->position.x - player->radius < -X_LIMIT) {
        player->position.x = -X_LIMIT + player->radius;
        player->velocity.x = 0;
    }
    // Empêcher de sortir à droite
    if (player->position.x + player->radius > X_LIMIT) {
        player->position.x = X_LIMIT - player->radius;
        player->velocity.x = 0;
    }

    for (int i = 0; i < nbPlatforms; i++) {
        resolveCircleRectCollision(player, platforms[i].rect);
    }

    // COYOTE TIME
    if (player->onGround) {
        player->coyoteTimer = COYOTE_TIME;
        player->nbJumps = 0;
    }
    else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0)
            player->coyoteTimer = 0;
    }

    // JUMP (buffer + coyote + double jump)
    if (player->jumpBuffer > 0) {
        // Jump sol ou coyote
        if (player->onGround || player->coyoteTimer > 0) {
            player->velocity.y = -500;

            PlaySound(sound_jump);

            player->onGround = false;
            player->coyoteTimer = 0;
            player->nbJumps = 1;
            player->jumpBuffer = 0;
        }
        // Double jump
        else if (player->nbJumps >= 1 && player->nbJumps < MAX_JUMPS) {
            player->velocity.y = -500;

            if (rand() % 1000 == 0) {
                PlaySound(meme);
            }
            else {
                PlaySound(sound_doubleJump);
            }

            player->nbJumps++;
            player->jumpBuffer = 0;
        }
    }
}

