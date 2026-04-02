/**
    @file core/game.c
    @author Fshimi-Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-02-08
    @date 2026-03-30
    @brief Player physics, collision, and advanced lobby mechanics.
*/

#include "core/game.h"
#include "utils/utils.h"
#include "utils/globals.h"
#include <math.h>

Rectangle getPlayerCollisionBox(const Player_St* const player) {
    return (Rectangle) {
        player->position.x - player->radius,
        player->position.y - player->radius,
        player->radius * 2,
        player->radius * 2
    };
}

Vector2 getPlayerCenter(const Player_St* const player) {
    return (Vector2) {player->radius, player->radius};
}

static bool isTerrainSolid(TerrainType_Et type) {
    switch (type) {
        case TERRAIN_NORMAL:
        case TERRAIN_WOOD:
        case TERRAIN_STONE:
        case TERRAIN_ICE:
        case TERRAIN_BOUNCY:
        case TERRAIN_MOVING_H:
        case TERRAIN_MOVING_V:
            return true;
        default:
            return false;
    }
}

f32 getWaterSubmersion(const Player_St* player, const Rectangle waterRect) {
    f32 playerBottom = player->position.y + player->radius;
    f32 waterTop     = waterRect.y;
    if (playerBottom <= waterTop) return 0.0f;
    f32 submergedDepth = playerBottom - waterTop;
    return clamp(submergedDepth / (player->radius * 2.0f), 0.0f, 1.0f);
}

static void resolvePlayerCircleVsTerrain(Player_St* player, const LobbyTerrain_St* currentTerrain) {
    TerrainType_Et type = currentTerrain->type;
    f32 closestX = clamp(player->position.x, currentTerrain->rect.x, currentTerrain->rect.x + currentTerrain->rect.width);
    f32 closestY = clamp(player->position.y, currentTerrain->rect.y, currentTerrain->rect.y + currentTerrain->rect.height);
    f32 hDelta = player->position.x - closestX;
    f32 vDelta = player->position.y - closestY;
    f32 distSq = hDelta * hDelta + vDelta * vDelta;
    f32 radius = player->radius;

    if (distSq >= radius * radius) return;

    if (type == TERRAIN_WATER) {
        player->isInWater = true;
        return;
    }
    if (!isTerrainSolid(type)) return;

    f32 distance = sqrtf(distSq);
    if (distance == 0.0f) return;
    f32 penetration = radius - distance;
    f32 normalX = hDelta / distance;
    f32 normalY = vDelta / distance;

    player->position.x += normalX * penetration;
    player->position.y += normalY * penetration;

    if (fabsf(normalX) > fabsf(normalY)) {
        player->velocity.x = 0.0f;
    } else {
        player->velocity.y = 0.0f;
        if (normalY < 0.0f) {
            player->onGround = true;
            player->nbJumps = 0;
            player->coyoteTimer = COYOTE_TIME;
            if (type == TERRAIN_BOUNCY) { player->velocity.y = -700.0f; player->onGround = false; }
            if (type == TERRAIN_ICE) player->onIce = true;
        }
    }
}

static void resolvePlayerVsAllTerrains(Player_St* player) {
    player->isInWater = false;
    player->onIce     = false;
    for (u32 i = 0; i < terrains.count; i++) {
        resolvePlayerCircleVsTerrain(player, &terrains.items[i]);
    }
}

void updatePlayer(LobbyGame_St* const game, const f32 dt) {
    Player_St* const player = &game->player;
    const PhysicsConstants_St* const pc = &game->physics[player->textureId];

    if (player->portalTeleportCooldown > 0.0f) {
        player->portalTeleportCooldown -= dt;
    }

    f32 submersion = 0.0f;
    if (player->isInWater) {
        for (u32 i = 0; i < terrains.count; ++i) {
            if (terrains.items[i].type == TERRAIN_WATER) {
                f32 s = getWaterSubmersion(player, terrains.items[i].rect);
                if (s > submersion) submersion = s;
            }
        }
    }

    f32 moveSpeed = pc->moveSpeed;
    if (IsKeyDown(KEY_A)) player->velocity.x = -moveSpeed;
    else if (IsKeyDown(KEY_D)) player->velocity.x = moveSpeed;
    else {
        f32 f = player->onIce ? pc->iceFriction : pc->friction;
        if (player->velocity.x > 0) player->velocity.x = max(0, player->velocity.x - f * dt);
        else if (player->velocity.x < 0) player->velocity.x = min(0, player->velocity.x + f * dt);
    }

    if (player->velocity.x > 0) player->angle += 360.0f * dt;
    else if (player->velocity.x < 0) player->angle -= 360.0f * dt;

    if (IsKeyPressed(KEY_SPACE)) player->jumpBuffer = pc->jumpBufferTime;
    else if (player->jumpBuffer > 0) player->jumpBuffer -= dt;

    if (submersion > 0.0f) {
        player->velocity.x *= pc->waterHorizDrag;
        player->velocity.y *= pc->waterVertDrag;
        f32 sink = IsKeyDown(KEY_S) ? pc->waterSinkWithS : pc->waterDefaultSink;
        f32 buoyancy = pc->waterBuoyancy * submersion * submersion;
        player->velocity.y += (buoyancy - sink) * dt;
    } else {
        player->velocity.y += pc->gravity * dt;
    }

    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;
    resolvePlayerVsAllTerrains(player);

    // Portals
    if (player->portalTeleportCooldown <= 0.0f) {
        for (u32 i = 0; i < terrains.count; ++i) {
            if (terrains.items[i].type == TERRAIN_PORTAL && !terrains.items[i].isOnlyReceiverPortal) {
                if (CheckCollisionCircleRec(player->position, player->radius, terrains.items[i].rect)) {
                    player->position = terrains.items[i].portalTargetPosition;
                    player->portalTeleportCooldown = 0.4f;
                    break;
                }
            }
        }
    }

    if (player->onGround) player->nbJumps = 0;
    if (player->jumpBuffer > 0.0f && (player->onGround || (player->isInWater && pc->waterInfiniteJump))) {
        player->velocity.y = pc->jumpForce;
        player->onGround = false;
        player->jumpBuffer = 0;
    }
}

void choosePlayerTexture(LobbyGame_St* const game) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = game->playerVisuals.defaultTextureRect;
        for (u32 i = 0; i < __playerTextureCount; ++i) {
            destRect.x = 20 + i * 60;
            if (game->player.unlockedTextures[i] && CheckCollisionPointRec(mousePos, destRect)) {
                game->player.textureId = i;
                game->playerVisuals.isTextureMenuOpen = false;
                break;
            }
        }
    }
}

void toggleSkinMenu(LobbyGame_St* const game) {
    if (IsKeyPressed(KEY_P) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), skinButtonRect))) {
        game->playerVisuals.isTextureMenuOpen = !game->playerVisuals.isTextureMenuOpen;
    }
}
