/**
    @file core/game.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-02-23
    @brief Player physics, collision, input handling and skin selection logic in the lobby.
*/

#include "core/game.h"

#include "utils/utils.h"
#include "utils/globals.h"

Rectangle getPlayerCollisionBox(const Player_st* const player) {
    return (Rectangle) {
        player->position.x - player->radius,
        player->position.y - player->radius,
        player->radius * 2,
        player->radius * 2
    };
}

Vector2 getPlayerCenter(const Player_st* const player) {
    return (Vector2) { player->radius, player->radius };
}

void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const f32 dt) {

    // Horizontal Input
    if (IsKeyDown(KEY_A)) {
        player->velocity.x = -300;
    } else if (IsKeyDown(KEY_D)) {
        player->velocity.x = 300;
    } else {
        if (player->velocity.x > 0) {
            player->velocity.x -= FRICTION * dt;
            if (player->velocity.x < 0) player->velocity.x = 0;
        } else if (player->velocity.x < 0) {
            player->velocity.x += FRICTION * dt;
            if (player->velocity.x > 0) player->velocity.x = 0;
        }
    }

    // Rotate depending on the player's direction
    if (player->velocity.x > 0)
        player->angle += 360 * dt;
    else if (player->velocity.x < 0)
        player->angle -= 360 * dt;

    // Buffered jump input
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    } else if (player->jumpBuffer > 0) {
        player->jumpBuffer = max(0, player->jumpBuffer - dt);
    }

    // Gravity
    player->velocity.y += GRAVITY * dt;

    // Move
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    for (int i = 0; i < nbPlatforms; i++) {
        resolveCircleRectCollision(player, platforms[i].rect);
    }

    // Coyote time
    if (player->onGround) {
        player->coyoteTimer = COYOTE_TIME;
        player->nbJumps = 0;
    } else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0)
            player->coyoteTimer = 0;
    }

    // Jump
    if (player->jumpBuffer > 0) {
        if (player->onGround || player->coyoteTimer > 0 || player->nbJumps < MAX_JUMPS) {
            player->velocity.y  = -JUMP_FORCE;
            player->onGround    = false;
            player->coyoteTimer = 0;
            player->nbJumps++;
            player->jumpBuffer  = 0;
        }
    }
}

/*
 * resolveCircleRectCollision
 *
 * Deux cas :
 *
 * Cas 1 — centre HORS du rectangle (normal).
 *   Point le plus proche sur le bord, push-out si dist < rayon.
 *
 * Cas 2 — centre DANS le rectangle (tunneling à grande vitesse).
 *   dist == 0 → pas de normale calculable avec la méthode standard.
 *   On calcule la pénétration minimale sur chaque axe et on éjecte
 *   par le côté le moins profond. Sans ce cas, la balle traverse le sol.
 */
void resolveCircleRectCollision(Player_st* player, Rectangle rect) {
    f32 centerX = player->position.x;
    f32 centerY = player->position.y;
    f32 r       = player->radius;

    f32 closestX = Clamp(centerX, rect.x, rect.x + rect.width);
    f32 closestY = Clamp(centerY, rect.y, rect.y + rect.height);

    f32 dx = centerX - closestX;
    f32 dy = centerY - closestY;
    f32 distSq = dx * dx + dy * dy;

    if (distSq > 0.0f) {
        /* Cas 1 : centre hors du rect */
        if (distSq >= r * r) return;

        f32 dist        = sqrtf(distSq);
        f32 penetration = r - dist;
        f32 nx = dx / dist;
        f32 ny = dy / dist;

        player->position.x += nx * penetration;
        player->position.y += ny * penetration;

        if (fabsf(nx) > fabsf(ny)) {
            player->velocity.x = 0;
        } else {
            player->velocity.y = 0;
            if (ny < 0) {
                player->onGround    = true;
                player->nbJumps     = 0;
                player->coyoteTimer = COYOTE_TIME;
            }
        }
    } else {
        /* Cas 2 : tunneling — centre à l'intérieur du rectangle */
        f32 overlapLeft   = centerX - rect.x;
        f32 overlapRight  = rect.x + rect.width  - centerX;
        f32 overlapTop    = centerY - rect.y;
        f32 overlapBottom = rect.y  + rect.height - centerY;

        f32 minOverlap = overlapLeft;
        f32 nx = -1.0f, ny = 0.0f;

        if (overlapRight  < minOverlap) { minOverlap = overlapRight;  nx =  1.0f; ny =  0.0f; }
        if (overlapTop    < minOverlap) { minOverlap = overlapTop;    nx =  0.0f; ny = -1.0f; }
        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; nx =  0.0f; ny =  1.0f; }

        player->position.x += nx * (minOverlap + r);
        player->position.y += ny * (minOverlap + r);

        if (fabsf(nx) > fabsf(ny)) {
            player->velocity.x = 0;
        } else {
            player->velocity.y = 0;
            if (ny < 0) {
                player->onGround    = true;
                player->nbJumps     = 0;
                player->coyoteTimer = COYOTE_TIME;
            }
        }
    }
}

void choosePlayerTexture(Player_st* player, LobbyGame_St* const game) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = game->playerVisuals.defaultTextureRect;

        for (int i = 0; i < __playerTextureCount; i++) {
            destRect.x = 20 + i * 60;
            if (!player->unlockedTextures[i]) continue;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->textureId = i;
                game->playerVisuals.isTextureMenuOpen = false;
                break;
            }
        }

        if (!game->playerVisuals.isTextureMenuOpen) return;
    }

    PlayerTextureId_Et selectedId = __playerTextureCount;

    struct {
        u32 keybind;
        u32 textureId;
    } keybindTextureIdAssociations[__playerTextureCount] = {
        { KEY_ONE,   PLAYER_TEXTURE_DEFAULT    },
        { KEY_TWO,   PLAYER_TEXTURE_EARTH      },
        { KEY_THREE, PLAYER_TEXTURE_TROLL_FACE },
    };

    u32 pressedKey = GetKeyPressed();

    for (u32 i = 0; i < __playerTextureCount; ++i) {
        if (pressedKey == keybindTextureIdAssociations[i].keybind) {
            selectedId = keybindTextureIdAssociations[i].textureId;
            break;
        }
    }

    if (selectedId == __playerTextureCount) return;

    if (!player->unlockedTextures[selectedId]) return;

    player->textureId = selectedId;
    game->playerVisuals.isTextureMenuOpen = false;
}

void toggleSkinMenu(LobbyGame_St* const game) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        game->playerVisuals.isTextureMenuOpen = !game->playerVisuals.isTextureMenuOpen;
    }
}
