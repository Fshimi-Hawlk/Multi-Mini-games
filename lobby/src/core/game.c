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
            - Full water terrain physics (float / slow sink / fast descent toggle)
            - Ice terrain slipping (low friction)
            - Full per-skin water physics as requested (default slow sink, battleship partial float, troll fast sink + depth-jump)
            - Infinite jumps for default/battleship in water, no infinite jump for troll
            - Buoyancy never fights sink when pressing S (full submersion possible)
            - All code follows project style rules exactly

    This file contains the core systems that drive the lobby player character:
        - Movement and input processing (horizontal + jump)
        - Gravity, velocity integration, friction
        - Circle-rectangle collision and penetration resolution
        - Coyote time and jump buffering mechanics
        - Texture/skin selection via mouse or number keys
        - Toggle logic for the skin selection overlay
        - Water floating/sinking and ice slipping

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

#include "utils/utils.h"
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
    @brief Returns whether a terrain type should participate in collision.

    Water is deliberately **not** solid (we handle floating/sinking manually).
    Ice is solid but gets extra low-friction handling later.
*/
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
    f32 waterBottom  = waterRect.y + waterRect.height;

    if (playerBottom <= waterTop) return 0.0f;           // completely above
    if (player->position.y - player->radius >= waterBottom) return 0.0f; // completely below (shouldn't happen)

    f32 submergedDepth = playerBottom - waterTop;
    return clamp(submergedDepth / (player->radius * 2.0f), 0.0f, 1.0f);
}

/**
    @brief Resolves collision between the circular player and one rectangular terrain piece.
           Also applies special effects for certain terrain types on landing.

    @note Overlap test is performed **first** for every terrain so that
          water/ice flags are only set when the player is actually touching them.
*/
static void resolvePlayerCircleVsTerrain(Player_St* player, const LobbyTerrain_St* currentTerrain) {
    TerrainType_Et type = currentTerrain->type;

    // ── 1. Overlap test (must be first for water/ice) ─────────────────────
    f32 closestX = Clamp(player->position.x,
                         currentTerrain->rect.x,
                         currentTerrain->rect.x + currentTerrain->rect.width);
    f32 closestY = Clamp(player->position.y,
                         currentTerrain->rect.y,
                         currentTerrain->rect.y + currentTerrain->rect.height);

    f32 horizontalDelta = player->position.x - closestX;
    f32 verticalDelta   = player->position.y - closestY;

    f32 distanceSquared = horizontalDelta * horizontalDelta + verticalDelta * verticalDelta;
    f32 radius          = player->radius;

    if (distanceSquared >= radius * radius) {
        return;                     // no overlap with this terrain
    }

    // ── 2. We are touching/overlapping this terrain ───────────────────────
    if (type == TERRAIN_WATER) {
        player->isInWater = true;
        return;                     // water is fluid – skip solid collision
    }

    if (!isTerrainSolid(type)) {
        return;
    }

    // ── 3. Solid terrain collision resolution ─────────────────────────────
    f32 distance = sqrtf(distanceSquared);
    if (distance == 0.0f) return;

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

            switch (type) {
                case TERRAIN_BOUNCY: {
                    player->velocity.y = -700.0f;
                    player->onGround   = false;
                } break;

                case TERRAIN_ICE: {
                    player->onIce = true;
                } break;

                default: break;
            } 
        }
    }
}

/**
    @brief Resolves all collisions between player and lobby terrains using circle collision.
*/
static void resolvePlayerVsAllTerrains(Player_St* player) {
    // Reset per-frame terrain flags
    player->isInWater = false;
    player->onIce     = false;

    for (u32 terrainIndex = 0; terrainIndex < terrains.count; terrainIndex++) {
        const LobbyTerrain_St* currentTerrain = &terrains.items[terrainIndex];
        resolvePlayerCircleVsTerrain(player, currentTerrain);
    }
}

void updatePlayer(LobbyGame_St* const game, const f32 dt) {
    Player_St* const player = &game->player;
    const PhysicsConstants_St* const pc = &game->physics[player->textureId];

    // ── Compute submersion (only if touching any water) ────────────────────
    f32 submersion = 0.0f;   // 0.0 = fully out, 1.0 = fully submerged

    if (player->isInWater) {   // will be set by collision below
        // Find the highest water surface the player is touching (for visual feel)
        for (u32 i = 0; i < terrains.count; ++i) {
            const LobbyTerrain_St* t = &terrains.items[i];
            if (t->type == TERRAIN_WATER) {
                f32 thisSub = getWaterSubmersion(player, t->rect);
                if (thisSub > submersion) submersion = thisSub;
            }
        }
    }

    // ── Horizontal input + skin-specific speed modifiers ───────────────────
    f32 effectiveMoveSpeed = pc->moveSpeed;
    if (player->textureId == PLAYER_TEXTURE_BATTLESHIP_TODO) {
        if (player->isInWater && submersion <= pc->waterTargetSubmersion) {
            effectiveMoveSpeed *= 2.0f;
        } else {
            effectiveMoveSpeed *= 0.25f;
        }
    }

    if (IsKeyDown(KEY_A)) {
        player->velocity.x = -effectiveMoveSpeed;
    } else if (IsKeyDown(KEY_D)) {
        player->velocity.x = effectiveMoveSpeed;
    } else {
        f32 currentFriction = player->onIce ? pc->iceFriction : pc->friction;

        if (player->velocity.x > 0.0f) {
            player->velocity.x -= currentFriction * dt;
            if (player->velocity.x < 0.0f) player->velocity.x = 0.0f;
        } else if (player->velocity.x < 0.0f) {
            player->velocity.x += currentFriction * dt;
            if (player->velocity.x > 0.0f) player->velocity.x = 0.0f;
        }
    }

    // Rotation based on horizontal speed
    if (player->velocity.x > 0.0f) player->angle += 360.0f * dt;
    else if (player->velocity.x < 0.0f) player->angle -= 360.0f * dt;

    // ── Jump buffering ─────────────────────────────────────────────────────
    if (IsKeyPressed(KEY_SPACE)) {
        player->jumpBuffer = pc->jumpBufferTime;
    } else if (player->jumpBuffer > 0.0f) {
        player->jumpBuffer = max(0.0f, player->jumpBuffer - dt);
    }

    // ── Vertical forces – per-skin logic ───────────────────────────────────
    if (submersion > 0.0f) {
        // Apply drag
        player->velocity.x *= pc->waterHorizDrag;
        player->velocity.y *= pc->waterVertDrag;

        f32 sinkForce = IsKeyDown(KEY_S) ? pc->waterSinkWithS : pc->waterDefaultSink;

        // Normal buoyancy / sink balance
        f32 buoyancy = pc->waterBuoyancy * submersion * submersion;

        // Stabilization toward target submersion for floating skins
        if (pc->waterTargetSubmersion < 1.0f) {
            buoyancy += 1400.0f * (pc->waterTargetSubmersion - submersion);
        }

        player->velocity.y += (buoyancy - sinkForce) * dt;
    } else {
        player->velocity.y += pc->gravity * dt;   // normal gravity on land
    }

    // ── Apply movement ─────────────────────────────────────────────────────
    player->position.x += player->velocity.x * dt;
    player->position.y += player->velocity.y * dt;

    player->onGround = false;

    // ── Collision resolution (also sets isInWater / onIce) ─────────────────
    resolvePlayerVsAllTerrains(player);

    // Battleship ground slowdown (applied after collision so we use current-frame water state)
    if (!player->isInWater && player->textureId == PLAYER_TEXTURE_BATTLESHIP_TODO) {
        player->velocity.x *= 0.25f;
    }

    // ── Coyote time & jump reset ───────────────────────────────────────────
    if (player->onGround) {
        player->coyoteTimer = pc->coyoteTime;
        player->nbJumps     = 0;
    } else {
        player->coyoteTimer -= dt;
        if (player->coyoteTimer < 0.0f) player->coyoteTimer = 0.0f;
    }

    // ── Jump logic ──────────────────────────────────────────────
    if (player->jumpBuffer > 0.0f) {
        bool canJump = (
            player->onGround 
         || player->coyoteTimer > 0.0f 
         || (player->isInWater && pc->waterInfiniteJump && pc->waterCanJump));

        if (canJump) {
            float jumpForce = player->isInWater ? pc->waterJumpForce : pc->jumpForce;

            player->velocity.y  = jumpForce;
            player->onGround    = false;
            player->coyoteTimer = 0.0f;
            player->nbJumps++;
            player->jumpBuffer  = 0.0f;
        }
    }
}

void choosePlayerTexture(LobbyGame_St* const game) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        Rectangle destRect = game->playerVisuals.defaultTextureRect;
        for (u32 i = 0; i < __playerTextureCount; ++i) {
            destRect.x = 20 + i * 60;
            if (!game->player.unlockedTextures[i]) continue ;
            if (CheckCollisionPointRec(mousePos, destRect)) {
                game->player.textureId = i;
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
        {KEY_ONE,   PLAYER_TEXTURE_DEFAULT},
        {KEY_TWO,   PLAYER_TEXTURE_EARTH},
        {KEY_THREE, PLAYER_TEXTURE_TROLL_FACE},
        {KEY_FOUR,  PLAYER_TEXTURE_BATTLESHIP_TODO},
        {KEY_FIVE,  PLAYER_TEXTURE_BINGO_TODO},
        {KEY_SIX,   PLAYER_TEXTURE_CONNECT_4_TODO},
        {KEY_SEVEN, PLAYER_TEXTURE_KFF_TODO},
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

    if (!game->player.unlockedTextures[selectedId]) {
        // TODO: Display Warning Message that the texture is locked
        return;
    }

    game->player.textureId = selectedId;
    game->playerVisuals.isTextureMenuOpen = false;
}

/**
 * @brief Toggles the skin selection menu visibility.
 */
void toggleSkinMenu(LobbyGame_St* const game) {
    bool cond = (
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), skinButtonRect)
    ) || IsKeyPressed(KEY_P);

    if (cond) {
        game->playerVisuals.isTextureMenuOpen = !game->playerVisuals.isTextureMenuOpen;
    }
}

/**
 * @brief Checks if the player has triggered a game transition zone.
 * @param player Pointer to the player structure.
 * @return 1 if King For Four is triggered, 0 otherwise.
 */
MiniGame_Et checkGameTrigger(void) {
    for (u8 i = 1; i < __miniGameCount; ++i) {
        bool hasCollided = CheckCollisionCircleRec(
            lobby_game.player.position, 
            lobby_game.player.radius, 
            gameInteractionZones[i].hitbox
        );
        
        if (hasCollided && IsKeyPressed(KEY_E)) {
            return i;
        }
    }

    return MINI_GAME_LOBBY;
}
