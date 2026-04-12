/**
    @file clientInterface.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-18
    @date 2026-03-30
    @brief Implementation of the Lobby mini-game module with Editor and Advanced Physics.
*/

#include "core/game.h"
#include "core/chat.h"

#include "setups/app.h"
#include "setups/audio.h"
#include "setups/game.h"
#include "setups/texture.h"

#include "ui/app.h"
#include "ui/game.h"
#include "ui/grass.h"
#include "ui/background.h"
#include "ui/ambiance.h"

#include "editor/editor.h"

#include "utils/globals.h"

#include "sharedUtils/mathUtils.h"

static f32Vector2 lastSentPos = {0};
static bool firstFrame = true;
static bool isFirstInit = true;

static void updateCameraOnWindowResize(LobbyGame_St* const game) {
    const f32 originalWidth  = 800;
    const f32 originalHeight = 600;

    // Always keep camera perfectly centered on the new window size
    game->cam.offset = (Vector2){
        systemSettings.video.width  / 2.0f,
        systemSettings.video.height / 2.0f
    };

    // ── Zoom adaptation when width OR height changes ─────────────────────
    f32 zoomX = (f32)systemSettings.video.width  / originalWidth;
    f32 zoomY = (f32)systemSettings.video.height / originalHeight;

    game->cam.zoom = min(zoomX, zoomY);
}

void lobby_init(void) {
    Error_Et error = OK;

    log_debug("[LOBBY]: Initializing client lobby");
    firstFrame = true;

    s32 savedId = isFirstInit ? -1 : lobby_game.clientId;
    isFirstInit = false;

    // Preserve the player name across lobby re-inits
    char savedName[32] = {0};
    strncpy(savedName, lobby_game.player.name, 31);

    memset(&lobby_game, 0, sizeof(lobby_game));
    lobby_game.clientId = savedId;

    // Video settings persist across lobby re-init.
    systemSettings = DEFAULT_SYSTEM_SETTING;
    systemSettings.video.title = "Lobby";
    error = applySystemSettings();
    if (error != OK) {
        log_error("System settings couldn't be applied correctly");
    }

    lobby_game.player = (Player_St) {
        .radius             = 20,
        .position           = {PLAYER_SPAWN_X, PLAYER_SPAWN_Y},
        .onGround           = true,
        .active             = true,
        .unlockedTextures   = {
            [PLAYER_TEXTURE_DEFAULT]        = true,
            [PLAYER_TEXTURE_BINGO]          = true,
            [PLAYER_TEXTURE_BOWLING]        = true,
            [PLAYER_TEXTURE_CHESS]          = true,
            [PLAYER_TEXTURE_KING_FOR_FOUR]  = true,
            [PLAYER_TEXTURE_LOBBY]          = true,
            [PLAYER_TEXTURE_SNAKE]          = true,
            [PLAYER_TEXTURE_SUIKA]          = true,
        },
    };

    // Restore the preserved name, or use the pseudo from the connection screen
    if (savedName[0] != '\0') {
        strncpy(lobby_game.player.name, savedName, 31);
    } else {
        extern const char* getEnteredPseudo(void);
        const char* _pseudo = getEnteredPseudo();
        strncpy(lobby_game.player.name, (_pseudo && _pseudo[0]) ? _pseudo : "Joueur", 31);
    }

    lobby_game.cam = (Camera2D) {
        .offset = { systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f },
        .target = {
            .x = lobby_game.player.position.x, 
            .y = lobby_game.player.position.y - lobby_game.player.radius * 1.5f
        },
        .zoom   = 1.0f,
    };
    
    lobby_game.playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    lobby_gameInit();
    lobby_initBackgroundScale();
    updateCameraOnWindowResize(&lobby_game);

    error = lobby_initTextures(lobby_game.playerVisuals.textures);

    // Initialize parameters menu (settings button)
    paramsMenu_init(&paramsMenu);

    // Physics constants for each skin
    for (s32 i = 0; i < __playerTextureCount; i++) {
        lobby_game.physics[i] = (PhysicsConstants_St){
            .gravity = GRAVITY, .airDrag = AIR_DRAG, .maxFallSpeed = MAX_FALL_SPEED,
            .moveSpeed = MOVE_SPEED, 
            .jumpForce = JUMP_FORCE, .coyoteTime = COYOTE_TIME, .jumpBufferTime = JUMP_BUFFER_TIME, .maxJumps = MAX_JUMPS,
            .friction = FRICTION, .iceFriction = ICE_FRICTION,
            .waterMaxSubmersion = 1.0f,
            .waterHorizDrag = WATER_HORIZ_DRAG, .waterVertDrag = WATER_VERT_DRAG, 
            .waterJumpForce = WATER_JUMP_FORCE, .waterCanJump = true,
        };
    }

    lobby_game.selectedTerrainIndex = -1;
    lobby_game.gridStep = 25.0f;
}

void lobby_on_data(s32 playerID, u8 action, const void* data, u16 len) {
    if (playerID < 0 || (playerID >= MAX_CLIENTS && playerID != 999)) {
        // Handle JOIN_ACK specifically if it comes from the server (which might use ID 0 or 999 depending on logic)
        if (action == ACTION_CODE_JOIN_ACK && len >= sizeof(u16)) {
            u16 tempID;
            memcpy(&tempID, data, sizeof(u16));
            lobby_game.clientId = ntohs(tempID);
            log_info("[LOBBY] My assigned ID: %d", lobby_game.clientId);
        }
        return;
    }
    if (playerID == lobby_game.clientId) return;

    if (playerID >= MAX_CLIENTS && action != ACTION_CODE_JOIN_ACK && action != ACTION_CODE_LOBBY_ROOM_INFO) {
        return; // Prevent OOB for server playerID (999)
    }

    switch (action) {
        case ACTION_CODE_JOIN_ACK: {
            u16 tempID;
            memcpy(&tempID, data, sizeof(u16)); 
            lobby_game.clientId = ntohs(tempID);
        } break;

        case ACTION_CODE_LOBBY_MOVE: {
            if (playerID >= MAX_CLIENTS) break;
            if (len < sizeof(PlayerNet_St)) break;
            PlayerNet_St net;
            memcpy(&net, data, sizeof(PlayerNet_St));
            Player_St* p = &lobby_game.otherPlayers[playerID];
            if (!p->active) {
                p->position = (Vector2){ net.x, net.y };
                p->targetPosition = p->position;
            } else {
                p->targetPosition = (Vector2){ net.x, net.y };
            }
            p->angle = net.angle;
            p->textureId = (PlayerTextureId_Et)net.textureId;
            p->active = net.active;
            p->radius = 20.0f;
            strncpy(p->name, net.name, 31);
        } break;

        case ACTION_CODE_LOBBY_CHAT: {
            if (len == 0) break;
            char safe_msg[MAX_CHAT_MSG_LEN];
            u16 copy_len = len > MAX_CHAT_MSG_LEN - 1 ? MAX_CHAT_MSG_LEN - 1 : len;
            memcpy(safe_msg, data, copy_len);
            safe_msg[copy_len] = '\0';
            
            char sender_name[32];
            snprintf(sender_name, sizeof(sender_name), "Joueur %d", playerID);
            if (playerID < MAX_CLIENTS && lobby_game.otherPlayers[playerID].active) {
                strncpy(sender_name, lobby_game.otherPlayers[playerID].name, 31);
                sender_name[31] = '\0';
            }
            
            addChatMessage(sender_name, safe_msg);
        } break;

        case ACTION_CODE_QUIT_GAME: {
            if (playerID < MAX_CLIENTS) {
                lobby_game.otherPlayers[playerID].active = false;
            }
        } break;
    }
}

void lobby_update(f32 dt) {
    updateChat();
    // toggleEditorMode removed: now handled by interaction zone in main.c

    if (lobby_game.chat.isOpen) {
        lobby_game.cam.target = lobby_game.player.position;
        return;
    }

    if (lobby_game.editorMode) {
        updateEditor(&lobby_game, dt);
        return;
    }

    gameTime += dt;

    if (IsWindowResized()) {
        systemSettings.video.width = GetScreenWidth();
        systemSettings.video.height = GetScreenHeight();

        skinButtonRect = (Rectangle) {
            .x = systemSettings.video.width - 70,
            .y = systemSettings.video.height / 2.0f - 25,
            .width = 50, 
            .height = 50
        };

        updateCameraOnWindowResize(&lobby_game);
    }

    lobby_updatePlayer(&lobby_game.player, &lobby_game.physics[lobby_game.player.textureId], dt);

    Vector2 desiredTarget = lobby_game.player.position;
    if (lobby_game.player.onGround && lobby_game.player.position.y > GROUND_Y - 70.0f) {
        desiredTarget.y -= 135.0f;
    } else {
        desiredTarget.y -= lobby_game.player.radius * 1.5f;
    }

    lobby_game.cam.target = Vector2Lerp(
        lobby_game.cam.target,
        desiredTarget,
        0.05f
    );

    paramsMenu_update(&paramsMenu);

    // Interpolate other players
    for (s32 i   = 0;   i < MAX_CLIENTS; i++) {
        if (lobby_game.otherPlayers[i].active && i != lobby_game.clientId) {
            Player_St* p = &lobby_game.otherPlayers[i];
            // Simple lerp for smooth movement
            p->position.x += (p->targetPosition.x - p->position.x) * 15.0f * dt;
            p->position.y += (p->targetPosition.y - p->position.y) * 15.0f * dt;
            
            // If very close, just snap
            if (CheckCollisionCircles(p->position, 1.0f, p->targetPosition, 1.0f)) {
                p->position = p->targetPosition;
            }
        }
    }

    lobby_toggleSkinMenu(&lobby_game.playerVisuals);

    if (lobby_game.playerVisuals.isTextureMenuOpen) {
        lobby_choosePlayerTexture(&lobby_game.playerVisuals, &lobby_game.player);
    }

    lobby_updateGrass(&lobby_game.player, GetFrameTime(), gameTime, lobby_game.cam);
    lobby_updateAtmosphericEffects(dt, &lobby_game.player, lobby_game.cam);


    if (lobby_game.player.position.x != lastSentPos.x || lobby_game.player.position.y != lastSentPos.y || firstFrame) {
        PlayerNet_St net = {
            .x = lobby_game.player.position.x, .y = lobby_game.player.position.y,
            .angle = lobby_game.player.angle, .textureId = (u8)lobby_game.player.textureId,
            .active = true
        };
        strncpy(net.name, lobby_game.player.name, 31);

        GameTLVHeader_St tlv = { .game_id = MINI_GAME_ID_LOBBY, .action = ACTION_CODE_LOBBY_MOVE, .length = htons(sizeof(PlayerNet_St)) };
        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
        h.sender_id = htons((u16)lobby_game.clientId);
        
        u8 buffer[1024];
        memcpy(buffer, &h, sizeof(h));
        memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
        memcpy(buffer + sizeof(h) + sizeof(tlv), &net, sizeof(net));
        
        send(networkSocket, buffer, sizeof(h) + sizeof(tlv) + sizeof(net), 0);
        lastSentPos = (f32Vector2){lobby_game.player.position.x, lobby_game.player.position.y};
        firstFrame = false;
    }
}

void lobby_draw(void) {
    BeginMode2D(lobby_game.cam); {
        lobby_drawStarryBackground(lobby_game.player.position, lobby_game.cam);
        lobby_drawTree();

        lobby_drawTerrains();
        lobby_drawPlayer(&lobby_game.playerVisuals, &lobby_game.player);

        for (s32 i   = 0;   i < MAX_CLIENTS; i++) {
            if (lobby_game.otherPlayers[i].active)
                lobby_drawPlayer(&lobby_game.playerVisuals, &lobby_game.otherPlayers[i]);
        }

        lobby_drawWorldBoundaries(&lobby_game.player);
        lobby_drawGrass(&lobby_game.player, lobby_game.cam);
        lobby_drawGameZones(&lobby_game.player);
        lobby_drawAtmosphericEffects();
    } EndMode2D();
    
    lobby_drawScreenEffects(&lobby_game.player);

    if (lobby_game.editorMode) {
        drawEditor(&lobby_game);
        return;
    }

    f32 lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
        DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

        lobby_drawSkinButton();
        if (lobby_game.playerVisuals.isTextureMenuOpen) {
            lobby_drawMenuTextures(&lobby_game);
        }
        
        drawChat();
}

void lobby_clientDestroy(void) {
    lobby_freeAudio();
    lobby_freeTextures(lobby_game.playerVisuals.textures);

    // Cleanup params menu
    paramsMenu_free(&paramsMenu);
}

GameClientInterface_St lobbyClientInterface = {
    .id         = MINI_GAME_ID_LOBBY,
    .name       = "Lobby",
    .init       = lobby_init,
    .on_data    = lobby_on_data,
    .update     = lobby_update,
    .draw       = lobby_draw,
    .destroy    = lobby_clientDestroy
};
