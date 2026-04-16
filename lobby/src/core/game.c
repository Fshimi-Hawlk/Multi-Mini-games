/**
    @file game.c
    @author Léandre BAUDET
    @date 2026-02-08
    @date 2026-04-14
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

Rectangle lobby_getPlayerCollisionBox(const Player_St* const player) {
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

static bool isTerrainSolid(TerrainKind_Et kind) {
    switch (kind) {
        case TERRAIN_KIND_NORMAL:
        case TERRAIN_KIND_GRASS:
        case TERRAIN_KIND_WOOD_PLANK:
        case TERRAIN_KIND_STONE:
        case TERRAIN_KIND_ICE:
        case TERRAIN_KIND_BOUNCY:
        case TERRAIN_KIND_MOVING_H:
        case TERRAIN_KIND_MOVING_V:
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
static void resolvePlayerCircleVsTerrain(Player_St* player, const PhysicsConstants_St* pc, const LobbyTerrain_St* t) {
    f32 centerX = player->position.x;
    f32 centerY = player->position.y;
    f32 r       = player->radius;

    TerrainKind_Et kind = t->kind;
    
    f32 closestX = clamp(centerX, t->rect.x, t->rect.x + t->rect.width);
    f32 closestY = clamp(centerY, t->rect.y, t->rect.y + t->rect.height);

    f32 dx = centerX - closestX;
    f32 dy = centerY - closestY;
    
    f32 distSq = dx * dx + dy * dy;

    if (distSq > 0.0f) { // First Case : player center outside of terrain
        if (distSq >= r * r) return;

        if (kind == TERRAIN_KIND_WATER) {
            player->isInWater = true;
            return;
        }

        if (!isTerrainSolid(kind)) {
            // Portals
            if (t->kind != TERRAIN_KIND_PORTAL || t->isOnlyReceiverPortal) return;
            if (player->portalTeleportCooldown <= 0.0f) {
                if (!CheckCollisionCircleRec(player->position, player->radius, t->rect)) return;

                player->position = t->portalTargetPosition;
                player->portalTeleportCooldown = 0.4f;
            }

            return;
        }

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
                player->coyoteTimer = pc->coyoteTime;

                switch (kind) {
                    case TERRAIN_KIND_BOUNCY: {
                        player->velocity.y = -700.0f; 
                        player->onGround = false;
                    } break;

                    case TERRAIN_KIND_ICE: player->onIce = true; break;

                    default: break;
                }
            }
        }
    } else {
        /* Cas 2 : tunneling — centre à l'intérieur du rectangle */
        f32 overlapLeft   = centerX - t->rect.x;
        f32 overlapRight  = t->rect.x + t->rect.width  - centerX;
        f32 overlapTop    = centerY - t->rect.y;
        f32 overlapBottom = t->rect.y  + t->rect.height - centerY;

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

static void resolvePlayerVsAllTerrains(Player_St* player, const PhysicsConstants_St* const pc) {
    player->isInWater = false;
    player->onIce     = false;

    for (u32 i = 0; i < terrains.count; i++) {
        resolvePlayerCircleVsTerrain(player, pc, &terrains.items[i]);
    }
}

void lobby_updatePlayer(Player_St* const player, const PhysicsConstants_St* const pc, const f32 dt) {
    if (player->portalTeleportCooldown > 0.0f) {
        player->portalTeleportCooldown -= dt;
    }

    // ── Compute water submersion ─────────────────────────────
    f32 submersion = 0.0f;
    if (player->isInWater) {
        for (u32 i = 0; i < terrains.count; ++i) {
            if (terrains.items[i].kind == TERRAIN_KIND_WATER) {
                f32 s = getWaterSubmersion(player, terrains.items[i].rect);
                if (s > submersion) submersion = s;
            }
        }
    }

    // ── Horizontal input + friction ───────────────
    if (IsKeyDown(KEY_A)) {
        player->velocity.x = -pc->moveSpeed;

    } else if (IsKeyDown(KEY_D)) {
        player->velocity.x = pc->moveSpeed;

    } else {
        f32 friction = player->onIce ? pc->iceFriction : pc->friction;
        
        if (player->velocity.x > 0.0f) {
            player->velocity.x = max(0.0f, player->velocity.x - friction * dt);
        } else if (player->velocity.x < 0.0f) {
            player->velocity.x = min(0.0f, player->velocity.x + friction * dt);
        }
    }

    if (IsKeyPressed(KEY_R)) {
        player->position = (f32Vector2) {PLAYER_SPAWN_X, PLAYER_SPAWN_Y};
        player->velocity = (f32Vector2) {0};
    }

    // ── Visual rotation ───────────────────────────────────────
    if (player->velocity.x != 0) {
        player->angle += (player->velocity.x > 0 ? 360 : -360) * dt;
    }

    // ── Jump buffering ────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = pc->jumpBufferTime;
    } else if (player->jumpBuffer > 0.0f) {
        player->jumpBuffer = max(0.0f, player->jumpBuffer - dt);
    }

    // ── Vertical forces (gravity / water) ─────────────────────────────────
    if (submersion > 0.0f) {
        player->velocity.x *= pc->waterHorizDrag;
        player->velocity.y *= pc->waterVertDrag;

        f32 sink = IsKeyDown(KEY_S) ? pc->waterSinkWithS : pc->waterDefaultSink;
        f32 buoyancy = pc->waterBuoyancy * submersion * submersion;

        player->velocity.y += (buoyancy - sink) * dt;

    } else { // outside water => apply gravity + air resistance + terminal velocity when in air
        player->velocity.y += pc->gravity * dt;

        // Fixes tunneling through floor from high jumps and slows down y-speed as requested
        if (!player->onGround && player->velocity.y > 0) {
            if (player->velocity.y > pc->maxFallSpeed) {
                player->velocity.y = pc->maxFallSpeed;
            }
            // Gentle linear drag for natural falling feel (very light)
            player->velocity.y *= (1.0f - AIR_DRAG * dt);
        }
    }

    // ── Integrate position ──────────────
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;
    player->onGround = false;

    // ── Collision Resolutions ───────────────
    resolvePlayerVsAllTerrains(player, pc);

    // ── World boundaries ──────────────────────────────────────────────────

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

    // ── Coyote time ───────────────────────────────────────────────────────
    if (player->onGround) {
        player->coyoteTimer = pc->coyoteTime;
        player->nbJumps = 0;
    } else if (player->coyoteTimer > 0) {
        player->coyoteTimer = max(0, player->coyoteTimer - dt);
    }

    // ── Jump ────────
    if (player->jumpBuffer > 0.0f) {
        bool canJump = (player->onGround || player->coyoteTimer > 0.0f || player->nbJumps < pc->maxJumps);
        bool inWaterJump = (player->isInWater && pc->waterInfiniteJump);

        if (canJump || inWaterJump) {
            if (player->nbJumps == 0) PlaySound(sound_jump);
            else PlaySound((rand() % 10000) == 0 ? sound_doubleJumpMeme : sound_doubleJump);

            player->velocity.y = -pc->jumpForce;
            player->onGround = false;
            player->coyoteTimer = 0;
            player->jumpBuffer = 0;
            player->nbJumps++;
        }
    }
}

void lobby_choosePlayerTexture(PlayerVisuals_St* const visuals, Player_St* const player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = visuals->defaultTextureRect;

        for (int i = 0; i < __playerTextureCount; i++) {
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

    static const int skinKeys[] = {
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE,
        KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE
    };

    for (u32 i = 0; i < __playerTextureCount && i < 9; ++i) {
        if (IsKeyPressed(skinKeys[i]) && player->unlockedTextures[i]) {
            player->textureId = (PlayerTextureId_Et)i;
            visuals->isTextureMenuOpen = false;
            break;
        }
    }
}

void lobby_toggleSkinMenu(PlayerVisuals_St* const visuals) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        visuals->isTextureMenuOpen = !visuals->isTextureMenuOpen;
    }
}
