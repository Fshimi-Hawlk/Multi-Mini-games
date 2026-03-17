/**
    @file core/game.c
    @author Maxime CHAUVEAU
    @date 2026-02-25
    @brief Player physics, collision, input handling and skin selection logic in the lobby.

    This file contains the core systems that drive the lobby player character:
        - Movement and input processing (horizontal + jump)
        - Gravity, velocity integration, friction
        - Circle-rectangle collision and penetration resolution
        - Coyote time and jump buffering mechanics
        - Texture/skin selection via mouse or number keys
        - Toggle logic for the skin selection overlay
        - Game zone detection and mini-game triggering

    All update functions expect:
        - dt in seconds (typically from GetFrameTime())
        - platforms in world coordinates
        - functions to be called once per frame in the main update loop

    Rendering-related helpers (getPlayerCollisionBox, getPlayerCenter) are used
    by draw routines and assume the player's collision shape is always a circle.

    @see `utils/types.h`     for `Player_st`, `LobbyGame_St`, `PlayerTexture_Et`
    @see `utils/configs.h`   for `FRICTION`, `COYOTE_TIME`, `JUMP_BUFFER_TIME`, `MAX_JUMPS`,
    @see `utils/globals.h`   for `skinButtonRect`, `gameZones`, `gameZoneCount`
    @see `core/game.h`       for `resolveCircleRectCollision()` declaration
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
    return (Vector2) {player->radius, player->radius};
}

void updatePlayer(Player_st* const player, const Platform_st* const platforms, const int nbPlatforms, const f32 dt) {

    // Horizontal Input
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        player->velocity.x = -MOVE_SPEED;
    }
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        player->velocity.x = MOVE_SPEED;
    }
    // Apply friction
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

    // Rotate depending on the player's direction
    if (player->velocity.x > 0) {
        player->angle += 360 * dt; // Clockwise
    }
    else if (player->velocity.x < 0) {
        player->angle -= 360 * dt; // Anti-clockwise
    }

    // Buffered jump input
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    }
    else if (player->jumpBuffer > 0) {
        player->jumpBuffer = max(0, player->jumpBuffer - dt);
    }

    // Gravity
    player->velocity.y += GRAVITY * dt;

    // Collision
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
    }
    else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0)
            player->coyoteTimer = 0;
    }

    // Jump => buffer + coyote + air jump(s)
    if (player->jumpBuffer > 0) {
        // Ground/Coyote Jump/Double jump
        if (player->onGround || player->coyoteTimer > 0 || player->nbJumps < MAX_JUMPS) {
            player->velocity.y = -JUMP_FORCE;
            player->onGround = false;
            player->coyoteTimer = 0;
            player->nbJumps++;
            player->jumpBuffer = 0;
        }
    }
}

void resolveCircleRectCollision(Player_st* player, Rectangle rect) {
    // Search the position that is closest to the circle on the rectangle
    f32 closestX = Clamp(player->position.x, rect.x, rect.x + rect.width);
    f32 closestY = Clamp(player->position.y, rect.y, rect.y + rect.height);

    f32 dx = player->position.x - closestX;
    f32 dy = player->position.y - closestY;

    f32 distSq = dx*dx + dy*dy;
    f32 r = player->radius;

    if (distSq >= r * r)
        return;

    f32 dist = sqrtf(distSq);
    if (dist == 0) {
        // Player is exactly at the closest point, push out along the axis with most movement
        if (fabsf(player->velocity.x) > fabsf(player->velocity.y)) {
            if (player->position.x < rect.x) player->position.x = rect.x - r;
            else if (player->position.x > rect.x + rect.width) player->position.x = rect.x + rect.width + r;
        } else {
            if (player->position.y < rect.y) player->position.y = rect.y - r;
            else if (player->position.y > rect.y + rect.height) player->position.y = rect.y + rect.height + r;
        }
        player->velocity.x = 0;
        player->velocity.y = 0;
        player->onGround = (player->velocity.y >= 0);
        return;
    }

    f32 penetration = r - dist;

    f32 nx = dx / dist;
    f32 ny = dy / dist;

    // Position correction
    player->position.x += nx * penetration;
    player->position.y += ny * penetration;

    // Speed resolution along the dominant axis
    if (fabsf(nx) > fabsf(ny)) {
        player->velocity.x = 0;
    }
    else {
        player->velocity.y = 0;

        // Ground
        if (ny < 0) {
            player->onGround = true;
            player->nbJumps = 0;
            player->coyoteTimer = COYOTE_TIME;
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
        {KEY_ONE, PLAYER_TEXTURE_DEFAULT},
        {KEY_TWO, PLAYER_TEXTURE_EARTH},
        {KEY_THREE, PLAYER_TEXTURE_TROLL_FACE},
        {KEY_FOUR, PLAYER_TEXTURE_BOWLING_BALL},
    };

    u32 pressedKey = GetKeyPressed();

    for (u32 i = 0; i < __playerTextureCount; ++i) {
        if (pressedKey == keybindTextureIdAssociations[i].keybind) {
            selectedId = keybindTextureIdAssociations[i].textureId;
            break;
        }
    }

    if (selectedId == __playerTextureCount) {
        return;
    }

    if (!player->unlockedTextures[selectedId]) {
        return;
    }

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

void checkGameZones(LobbyGame_St* const game) {
    for (u8 i = 1; i < __gameSceneCount; ++i) {
        if (CheckCollisionCircleRec(game->player.position, game->player.radius, game->subGameManager.gameHitboxes[i])) {
            if (!game->subGameManager.gameHitGracePeriodActive) {
                // Player enters a game zone - check for interaction key
                if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
                    game->subGameManager.currentScene = i;
                    game->subGameManager.needGameInit = true;
                    game->subGameManager.gameHitGracePeriodActive = true;
                }
            }
        } else if (game->subGameManager.gameHitGracePeriodActive) {
            game->subGameManager.gameHitGracePeriodActive = false;
        }
    }
}

bool isNearGameZone(const Player_st* const player, const GameZone_st* const zones, const u32 count) {
    for (u32 i = 0; i < count; ++i) {
        Vector2 zoneCenter = {
            zones[i].hitbox.x + zones[i].hitbox.width / 2.0f,
            zones[i].hitbox.y + zones[i].hitbox.height / 2.0f
        };
        f32 dist = Vector2Distance(player->position, zoneCenter);
        if (dist < GAME_ZONE_PROXIMITY_THRESHOLD) {
            return true;
        }
    }
    return false;
}
