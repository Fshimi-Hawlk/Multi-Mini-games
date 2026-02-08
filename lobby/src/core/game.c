#include "core/game.h"
#include "utils/utils.h"
#include "utils/globals.h"

void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const float dt) {

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
        } 
        else if (player->velocity.x < 0) {
            player->velocity.x += FRICTION * dt;
            if (player->velocity.x > 0) player->velocity.x = 0;
        }
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
        player->jumpBuffer = max(0, player->jumpBuffer - dt);
    }

    // GRAVITÉ
    player->velocity.y += 1200 * dt;

    // COLLISIONS
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

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

void resolveCircleRectCollision(Player_st* player, Rectangle rect) {
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


void choosePlayerTexture(Player_st* player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = defaultPlayerTextureRect;
    
        for (int i = 0; i < playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->texture = &playerTextures[i];
                isTextureMenuOpen = false;
                break;
            }
        }
    }
    if (IsKeyPressed(KEY_ONE)) {
        player->texture = &playerTextures[0];
        isTextureMenuOpen = false;
    }
    if (IsKeyPressed(KEY_TWO)) {
        player->texture = &playerTextures[1];
        isTextureMenuOpen = false;
    }
}

void toggleSkinMenu(void) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        isTextureMenuOpen = !isTextureMenuOpen;
    }
}