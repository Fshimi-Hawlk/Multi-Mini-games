/**
 * @file game.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of physics and player logic.
 */

#include "core/game.h"
#include "utils/globals.h"
#include "raymath.h"
#include <math.h>

/**
 * @brief Updates player movement and physics.
 */
void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const float dt) {
    // 1. Input Horizontaux
    float moveX = 0;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) moveX -= 1.0f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) moveX += 1.0f;
    
    player->velocity.x = moveX * 400.0f;

    // 2. Gravité
    if (!player->onGround) {
        player->velocity.y += GRAVITY * dt;
        player->coyoteTimer -= dt;
    } else {
        player->velocity.y = 0;
        player->coyoteTimer = COYOTE_TIME;
    }

    // 3. Saut
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        player->jumpBuffer = 0.15f;
    }
    if (player->jumpBuffer > 0) player->jumpBuffer -= dt;

    if (player->jumpBuffer > 0) {
        if (player->onGround || player->coyoteTimer > 0) {
            player->velocity.y = -500.0f;
            player->onGround = false;
            player->coyoteTimer = 0;
            player->jumpBuffer = 0;
            player->nbJumps = 1; // Premier saut fait
        } else if (player->nbJumps < 2) {
            player->velocity.y = -500.0f;
            player->nbJumps = 2; // Double saut fait
            player->jumpBuffer = 0;
        }
    }

    // 4. Application mouvement & Collisions
    player->position.x += player->velocity.x * dt;
    for (int i = 0; i < nbPlatforms; i++) resolveCircleRectCollision(player, platforms[i].rect);

    // Animation: rotation based on velocity (converted to degrees for Raylib)
    if (fabsf(player->velocity.x) > 0.1f) {
        player->angle += (player->velocity.x * dt) * 1.0f; // Multiplier pour l'effet visuel
    }

    player->position.y += player->velocity.y * dt;
    player->onGround = false;
    for (int i = 0; i < nbPlatforms; i++) resolveCircleRectCollision(player, platforms[i].rect);
}

/**
 * @brief Resolves collision between player and platform.
 */
void resolveCircleRectCollision(Player_st* const player, const Rectangle rect) {
    float nearestX = fmaxf(rect.x, fminf(player->position.x, rect.x + rect.width));
    float nearestY = fmaxf(rect.y, fminf(player->position.y, rect.y + rect.height));

    float dx = player->position.x - nearestX;
    float dy = player->position.y - nearestY;
    float distance = sqrtf(dx*dx + dy*dy);

    if (distance >= player->radius || distance == 0) return;

    float overlap = player->radius - distance;
    float nx = dx / distance;
    float ny = dy / distance;

    player->position.x += nx * overlap;
    player->position.y += ny * overlap;

    if (fabsf(nx) > fabsf(ny)) {
        player->velocity.x = 0;
    } else {
        player->velocity.y = 0;
        if (ny < 0) {
            player->onGround = true;
            player->nbJumps = 0;
        }
    }
}

/**
 * @brief Cycles through available player textures based on user input.
 */
void choosePlayerTexture(Player_st* player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = defaultPlayerTextureRect;

        for (int i = 0; i < playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->texture = &playerTextures[i];
                player->skin_id = i;
                isTextureMenuOpen = false;
                break;
            }
        }
    }
    if (IsKeyPressed(KEY_ONE)) {
        player->texture = &playerTextures[0];
        player->skin_id = 0;
    }
    if (IsKeyPressed(KEY_TWO) && playerTextureCount > 1) {
        player->texture = &playerTextures[1];
        player->skin_id = 1;
    }
}

/**
 * @brief Toggles the skin selection menu visibility.
 */
void toggleSkinMenu(void) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        isTextureMenuOpen = !isTextureMenuOpen;
    }
}

/**
 * @brief Checks if the player has triggered a game transition zone.
 */
int checkGameTrigger(Player_st* player) {
    if (CheckCollisionCircleRec(player->position, player->radius, kingForFourZone)) {
        return 1; 
    }
    return 0; 
}
