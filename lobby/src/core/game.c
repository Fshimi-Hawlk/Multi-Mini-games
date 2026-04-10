/**
    @file core/game.c
    @author Fshimi-Hawlk
    @date 2026-02-08
    @date 2026-04-08
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

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"

Rectangle getPlayerCollisionBox(const Player_St* const player) {
    return (Rectangle) {
        player->position.x - player->radius,
        player->position.y - player->radius,
        player->radius * 2,
        player->radius * 2
    };
}

Vector2 lobby_getPlayerCenter(const Player_St* const player) {
    return (Vector2) {player->radius, player->radius};
}

/**
    @brief Resolves collision between player's circle and a single axis-aligned rectangle.

    Performs:
        - closest-point calculation
        - penetration depth computation
        - position correction (push out)
        - velocity nulling along dominant axis
        - ground detection (sets onGround, resets jumps/coyote when landing from above)

    @param player  Player state (position and velocity are modified)
    @param rect    Rectangle to collide against
*/
static void resolveCircleRectCollision(Player_St* player, const Rectangle rect) {
    f32 centerX = player->position.x;
    f32 centerY = player->position.y;
    f32 r       = player->radius;

    // Search the position that is closest to the circle on the rectangle
    f32 closestX = Clamp(player->position.x, rect.x, rect.x + rect.width);
    f32 closestY = Clamp(player->position.y, rect.y, rect.y + rect.height);

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

void lobby_updatePlayer(Player_St* const player, const Platform_St* const platforms, const int nbPlatforms, const f32 dt) {
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

    if (IsKeyPressed(KEY_R)) {
        player->position = (f32Vector2) {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};
        player->velocity = (f32Vector2) {0};
    }

    // Rotate depending on the player's direction
    if (player->velocity.x > 0) {
        player->angle += 360 * dt; // Clockwise
    }
    else if (player->velocity.x < 0) {
        player->angle -= 360 * dt; // Anti-clockwise
    }

    // Buffered jump input
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = JUMP_BUFFER_TIME;
    } else if (player->jumpBuffer > 0) {
        player->jumpBuffer = max(0, player->jumpBuffer - dt);
    }

    // Gravity
    player->velocity.y += 1200 * dt;

    // Air resistance + terminal velocity when in air
    // Fixes tunneling through floor from high jumps and slows down y-speed as requested
    if (!player->onGround && player->velocity.y > 0) {
        if (player->velocity.y > MAX_FALL_SPEED) {
            player->velocity.y = MAX_FALL_SPEED;
        }
        // Gentle linear drag for natural falling feel (very light)
        player->velocity.y *= (1.0f - AIR_DRAG * dt);
    }

    // Collision
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    for (int i = 0; i < nbPlatforms; i++) {
        resolveCircleRectCollision(player, platforms[i].rect);
    }

    // Left border
    if (player->position.x - player->radius < -X_LIMIT) {
        player->position.x = -X_LIMIT + player->radius;
        player->velocity.x = 0;
    }

    // Right border
    if (player->position.x + player->radius > X_LIMIT) {
        player->position.x = X_LIMIT - player->radius;
        player->velocity.x = 0;
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
            player->jumpBuffer = 0;
        }
    }
}

void lobby_choosePlayerTexture(Player_St* player, LobbyGame_St* const game) {
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

void lobby_toggleSkinMenu(LobbyGame_St* const game) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        game->playerVisuals.isTextureMenuOpen = !game->playerVisuals.isTextureMenuOpen;
    }
}
