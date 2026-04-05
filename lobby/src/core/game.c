/**
    @file core/game.c
    @author Fshimi-Hawlk
    @author LeandreB8
    @author i-Charlys (CAILLON Charles)
    @date 2026-02-08
    @date 2026-03-18
    @brief Player physics, collision, input handling and skin selection logic in the lobby.

    Contributors:
        - LeandreB8:
            - Circle-vs-rectangle collision with basic resolution and ground detection
            - Horizontal movement with friction, rotation based on direction
            - Coyote time, jump buffering, limited air jumps
            - Mouse + keybind driven skin selection
            - Skin menu toggle
        - Fshimi-Hawlk:
            - Moved the game logic off main to this file
            - Reworked player texture logic
            - Provided documentation

    This file contains the core systems that drive the lobby player character:
        - Movement and input processing (horizontal + jump)
        - Gravity, velocity integration, friction
        - Circle-rectangle collision and penetration resolution
        - Coyote time and jump buffering mechanics
        - Texture/skin selection via mouse or number keys
        - Toggle logic for the skin selection overlay

    All update functions expect:
        - dt in seconds (typically from GetFrameTime())
        - platforms in world coordinates
        - functions to be called once per frame in the main update loop

    Rendering-related helpers (getPlayerCollisionBox, getPlayerCenter) are used
    by draw routines and assume the player's collision shape is always a circle.

    @see `utils/userTypes.h`     for `Player_St`, `LobbyGame_St`, `PlayerTexture_Et`
    @see `utils/configs.h`       for `FRICTION`, `COYOTE_TIME`, `JUMP_BUFFER_TIME`, `MAX_JUMPS`,
    @see `utils/globals.h`       for `skinButtonRect`
    @see `core/game.h`           for `resolveCircleRectCollision()` declaration
*/

#include "core/game.h"

#include "utils/mathUtils.h"
#include "utils/globals.h"

Rectangle getPlayerCollisionBox(const Player_St* const player) {
    return (Rectangle) {
        player->position.x,
        player->position.y,
        player->radius * 2,
        player->radius * 2
    };
}

Vector2 getPlayerCenter(const Player_St* const player) {
    return (Vector2) {player->radius, player->radius};
}

/**
    @brief Resolves collision between the circular player and one rectangular terrain piece.
           Also applies special effects for certain terrain types on landing.
*/
static void resolvePlayerCircleVsTerrain(Player_St* player, const LobbyTerrain_St* currentTerrain) {
    // Find closest point on the rectangle to the circle center
    f32 closestX = Clamp(player->position.x, currentTerrain->rect.x, currentTerrain->rect.x + currentTerrain->rect.width);
    f32 closestY = Clamp(player->position.y, currentTerrain->rect.y, currentTerrain->rect.y + currentTerrain->rect.height);

    f32 horizontalDelta = player->position.x - closestX;
    f32 verticalDelta   = player->position.y - closestY;

    f32 distanceSquared = horizontalDelta * horizontalDelta + verticalDelta * verticalDelta;
    f32 radius          = player->radius;

    if (distanceSquared >= radius * radius) {
        return;
    }

    f32 distance = sqrtf(distanceSquared);
    if (distance == 0.0f) {
        return;
    }

    f32 penetration = radius - distance;

    f32 normalX = horizontalDelta / distance;
    f32 normalY = verticalDelta   / distance;

    // Position correction
    player->position.x += normalX * penetration;
    player->position.y += normalY * penetration;

    // Velocity resolution
    if (fabsf(normalX) > fabsf(normalY)) {
        player->velocity.x = 0.0f;
    } else {
        player->velocity.y = 0.0f;

        // Landing on ground
        if (normalY < 0.0f) {
            player->onGround = true;
            player->nbJumps  = 0;
            player->coyoteTimer = COYOTE_TIME;
        }
    }
}

/**
    @brief Resolves all collisions between player and lobby terrains using circle collision.
*/
static void resolvePlayerVsAllTerrains(Player_St* player, const TerrainVec_St terrains) {
    for (u32 terrainIndex = 0; terrainIndex < terrains.count; terrainIndex++) {
        const LobbyTerrain_St* currentTerrain = &terrains.items[terrainIndex];
        resolvePlayerCircleVsTerrain(player, currentTerrain);
    }
}

void updatePlayer(Player_St* const player, const f32 dt) {
    // Horizontal input + friction
    if (IsKeyDown(KEY_A)) {
        player->velocity.x = -300.0f;
    } else if (IsKeyDown(KEY_D)) {
        player->velocity.x = 300.0f;
    } else {
        f32 friction = FRICTION;

        if (player->velocity.x > 0.0f) {
            player->velocity.x -= friction * dt;
            if (player->velocity.x < 0.0f) player->velocity.x = 0.0f;
        } else if (player->velocity.x < 0.0f) {
            player->velocity.x += friction * dt;
            if (player->velocity.x > 0.0f) player->velocity.x = 0.0f;
        }
    }

    // Rotation based on horizontal speed
    if (player->velocity.x > 0.0f) {
        player->angle += 360.0f * dt;
    } else if (player->velocity.x < 0.0f) {
        player->angle -= 360.0f * dt;
    }

    // Jump buffering
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    } else if (player->jumpBuffer > 0.0f) {
        player->jumpBuffer = max(0.0f, player->jumpBuffer - dt);
    }

    // Gravity
    player->velocity.y += 1200.0f * dt;

    // Apply movement
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    player->onGround = false;

    // Collision resolution (circle vs all solid terrains)
    resolvePlayerVsAllTerrains(player, terrains);

    // Coyote time & jump reset
    if (player->onGround) {
        player->coyoteTimer = COYOTE_TIME;
        player->nbJumps     = 0;
    } else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0.0f) player->coyoteTimer = 0.0f;
    }

    // Jump logic
    if (player->jumpBuffer > 0.0f) {
        bool canJump = player->onGround || player->coyoteTimer > 0.0f || player->nbJumps < MAX_JUMPS;

        if (canJump) {
            player->velocity.y  = -500.0f;
            player->onGround    = false;
            player->coyoteTimer = 0.0f;
            player->nbJumps++;
            player->jumpBuffer  = 0.0f;
        }
    }
}

void choosePlayerTexture(Player_St* const player, PlayerVisuals_St* const visuals) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = visuals->defaultTextureRect;
        for (u32 i = 0; i < __playerTextureCount; ++i) {
            destRect.x = 20 + i * 60;
            if (!player->unlockedTextures[i]) continue;

            if (CheckCollisionPointRec(mousePos, destRect)) {
                player->textureId = i;
                visuals->isTextureMenuOpen = false;
                break;
            }
        }

        if (!visuals->isTextureMenuOpen) return;
    }

    PlayerTextureId_Et selectedId = __playerTextureCount;

    struct {
        u32 keybind;
        u32 textureId;
    } keybindTextureIdAssociations[__playerTextureCount] = {
        {KEY_ONE,   PLAYER_TEXTURE_DEFAULT},
        {KEY_TWO,   PLAYER_TEXTURE_EARTH},
        {KEY_THREE, PLAYER_TEXTURE_TROLL_FACE},
        {KEY_FOUR,  PLAYER_TEXTURE_BATTLESHIP_TODO},
        {KEY_FIVE,  PLAYER_TEXTURE_BINGO},
        {KEY_SIX,   PLAYER_TEXTURE_CONNECT_4_TODO},
        {KEY_SEVEN, PLAYER_TEXTURE_KFF},
        {KEY_EIGHT, PLAYER_TEXTURE_MINIGOLF_TODO},
        {KEY_NINE,  PLAYER_TEXTURE_MORPION_TODO},
        {KEY_ZERO,  PLAYER_TEXTURE_OTHELLO_TODO},
    };

    u32 pressedKey = GetKeyPressed();

    for (u32 i = 0; i < __playerTextureCount; ++i) {
        if (pressedKey == keybindTextureIdAssociations[i].keybind) {
            selectedId = keybindTextureIdAssociations[i].textureId;
            break;
        }
    }

    if (selectedId == __playerTextureCount) {
        // TODO: Display Error Message if necessary
        return;
    }

    if (!player->unlockedTextures[selectedId]) {
        // TODO: Display Warning Message that the texture is locked
        return;
    }

    player->textureId = selectedId;
    visuals->isTextureMenuOpen = false;
}

/**
 * @brief Toggles the skin selection menu visibility.
 */
void toggleSkinMenu(PlayerVisuals_St* const visuals) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        visuals->isTextureMenuOpen = !visuals->isTextureMenuOpen;
    }
}

MiniGame_Et checkGameTrigger(const Player_St* const player) {
    for (u8 i = 0; i < __miniGameCount; ++i) {
        if (i == MINI_GAME_LOBBY) continue;

        bool hasCollided = CheckCollisionCircleRec(
            player->position, 
            player->radius, 
            gameInteractionZones[i].hitbox
        );
        
        if (hasCollided && IsKeyPressed(KEY_E)) {
            return i;
        }
    }

    return MINI_GAME_LOBBY;
}
