/**
 * @file game.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of core game logic for the lobby (physics and collisions).
 */

#include "core/game.h"

#include "utils/utils.h"
#include "utils/globals.h"

/**
 * @brief Updates the player's position and state based on input and collisions.
 * @param player Pointer to the player structure to update.
 * @param platforms Array of platforms for collision detection.
 * @param nbPlatforms Number of platforms in the array.
 * @param dt Delta time since the last frame.
 */
void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const float dt) {

    if (IsKeyDown(KEY_A))
        player->velocity.x = -300;
    else if (IsKeyDown(KEY_D))
        player->velocity.x = 300;
    else {
        if (player->velocity.x > 0) {
            player->velocity.x -= FRICTION * dt;
            if (player->velocity.x < 0) player->velocity.x = 0;
        }
        else if (player->velocity.x < 0) {
            player->velocity.x += FRICTION * dt;
            if (player->velocity.x > 0) player->velocity.x = 0;
        }
    }

    if (player->velocity.x > 0) {
        player->angle += 360 * dt; 
    } 
    else if (player->velocity.x < 0) {
        player->angle -= 360 * dt; 
    }

    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    }
    else if (player->jumpBuffer > 0) {
        player->jumpBuffer = max(0, player->jumpBuffer - dt);
    }

    player->velocity.y += 1200 * dt;

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    for (int i = 0; i < nbPlatforms; i++) {
        resolveCircleRectCollision(player, platforms[i].rect);
    }

    if (player->onGround) {
        player->coyoteTimer = COYOTE_TIME;
        player->nbJumps = 0;
    }
    else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0)
            player->coyoteTimer = 0;
    }

    if (player->jumpBuffer > 0) {
        if (player->onGround || player->coyoteTimer > 0) {
            player->velocity.y = -500;
            player->onGround = false;
            player->coyoteTimer = 0;
            player->nbJumps = 1;
            player->jumpBuffer = 0;
        }
        // Double jump
        // adding: `player->nbJumps >= 1` to the below cond makes that player can't
        // air jump if they haven't already jump previously
        else if (player->nbJumps < MAX_JUMPS) {
            player->velocity.y = -500;
            player->nbJumps++;
            player->jumpBuffer = 0;
        }
    }
}

/**
 * @brief Resolves collision between a circular player and a rectangular obstacle.
 * @param player Pointer to the player structure.
 * @param rect The rectangle to check collision against.
 */
void resolveCircleRectCollision(Player_st* player, Rectangle rect) {
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

    player->position.x += nx * penetration;
    player->position.y += ny * penetration;

    if (fabsf(nx) > fabsf(ny)) {
        player->velocity.x = 0;
    }
    else {
        player->velocity.y = 0;

        if (ny < 0) {
            player->onGround = true;
            player->nbJumps = 0;
            player->coyoteTimer = COYOTE_TIME;
        }
    }
}

/**
 * @brief Cycles through available player textures based on user input.
 * @param player Pointer to the player structure to update.
 */
void choosePlayerTexture(Player_st* player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = player->visuals.defaultTextureRect;

        for (int i = 0; i < __playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->visuals.textureId = i;
                isTextureMenuOpen = false;
                break;
            }
        }

        if (!isTextureMenuOpen) return;
    }

    switch (GetKeyPressed()) {
        case KEY_ONE: {
            player->visuals.textureId = PLAYER_TEXTURE_DEFAULT;
            isTextureMenuOpen = false;
        } break;

        case KEY_TWO: {
            player->visuals.textureId = PLAYER_TEXTURE_EARTH;
            isTextureMenuOpen = false;
        } break;

        case KEY_THREE: {
            player->visuals.textureId = PLAYER_TEXTURE_TROLL_FACE;
            isTextureMenuOpen = false;
        } break;
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
 * @param player Pointer to the player structure.
 * @return 1 if King For Four is triggered, 0 otherwise.
 */
int checkGameTrigger(Player_st* player) {
    if (CheckCollisionCircleRec(player->position, player->radius, kingForFourZone)) {
        return 1; 
    }
    return 0; 
}