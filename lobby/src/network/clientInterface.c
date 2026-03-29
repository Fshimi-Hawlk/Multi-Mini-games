/**
    @file clientInterface.c
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-03-27
    @brief Implementation of the Lobby mini-game module.

    Contributors:
        - i-Charlys (CAILLON Charles):
            - Original implementation
        - Fshimi-Hawlk:
            - Refactored the whole code
            - Added level editor logic
            - Added initialization of new water/ice physics fields in `lobby_init`
*/

#include "core/game.h"
#include "core/chat.h"

#include "ui/game.h"
#include "ui/app.h"
#include "ui/editor.h"

#include "utils/globals.h"
#include "utils/utils.h"

#include "systemSettings.h"

LobbyGame_St lobby_game = {0};

static f32Vector2 lastSentPos = {0};
static bool firstFrame = true;

/**
 * @brief Initializes the lobby module (loads textures, sets camera).
 */
void lobby_init(void) {
    log_debug("[LOBBY]: Initializing client lobby");
    
    Error_Et error = OK;
    memset(&lobby_game, 0, sizeof(lobby_game));

    systemSettings.video.height = DEFAULT_VIDEO_SETTING_HEIGHT;
    systemSettings.video.width = DEFAULT_VIDEO_SETTING_WIDTH;
    applySystemSettings();

    /** Player controlled by the user in the lobby */
    lobby_game.player = (Player_St) {
        .position         = {0, 0},
        .radius           = 20,
        .coyoteTime       = 0.1f,
        .coyoteTimer      = 0.1f,
        .unlockedTextures = {
            [PLAYER_TEXTURE_DEFAULT] = true,
            [PLAYER_TEXTURE_EARTH] = true,
            [PLAYER_TEXTURE_TROLL_FACE] = true,
            [PLAYER_TEXTURE_BATTLESHIP_TODO] = true,
        },
        .isInWater        = false,
        .waterFastDescent = false,
        .onIce            = false
    };

    /** Camera following the player in 2D mode */
    lobby_game.cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };
    
    lobby_game.playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    // Load shared UI textures
    const char* playerTextureImagePaths[__playerTextureCount] = {
        [PLAYER_TEXTURE_EARTH]      = IMAGES_PATH "earth.png",
        [PLAYER_TEXTURE_TROLL_FACE] = IMAGES_PATH "trollFace.png",
    };

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        const char* path = playerTextureImagePaths[i];
        if (path == NULL) continue;

        lobby_game.playerVisuals.textures[i] = LoadTexture(path);
        if (!IsTextureValid(lobby_game.playerVisuals.textures[i])) {
            log_warn("%s couldn't be loaded proprely.", path);
            error =  ERROR_TEXTURE_LOAD;
        }
    }

    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }

    lobby_game.currentState = GAME_STATE_GAMEPLAY;

    lobby_game.selectedTerrainIndex = -1;
    lobby_game.isDragging = false;
    lobby_game.gridStep = 25.0f;

    PhysicsConstants_St physics[__playerTextureCount] = {
        [PLAYER_TEXTURE_DEFAULT] = {
            .gravity        = GRAVITY,
            .moveSpeed      = MOVE_SPEED,
            .jumpForce      = JUMP_FORCE,
            .coyoteTime     = COYOTE_TIME,
            .jumpBufferTime = JUMP_BUFFER_TIME,
            .maxJumps       = MAX_JUMPS,
            .friction       = FRICTION,
            .iceFriction    = ICE_FRICTION,

            .waterBuoyancy          = 0.0f,
            .waterDefaultSink       = -320.0f,
            .waterSinkWithS         = -820.0f,
            .waterHorizDrag         = WATER_HORIZ_DRAG,
            .waterVertDrag          = WATER_VERT_DRAG,
            .waterJumpForce         = WATER_JUMP_FORCE,
            .waterTargetSubmersion  = 1.0f,
            .waterMaxSubmersion     = 1.0f,
            .waterCanJump           = true,
            .waterInfiniteJump      = true,
            .waterAlwaysFloat       = false,
        },
        [PLAYER_TEXTURE_EARTH] = {
            .gravity        = 0.0f,
            .moveSpeed      = MOVE_SPEED,
            .jumpForce      = JUMP_FORCE,
            .coyoteTime     = COYOTE_TIME,
            .jumpBufferTime = JUMP_BUFFER_TIME,
            .maxJumps       = MAX_JUMPS,
            .friction       = FRICTION,
            .iceFriction    = ICE_FRICTION,

            .waterBuoyancy          = WATER_BUOYANCY,
            .waterDefaultSink       = 0.0f,
            .waterSinkWithS         = 0.0f,
            .waterHorizDrag         = WATER_HORIZ_DRAG,
            .waterVertDrag          = WATER_VERT_DRAG,
            .waterJumpForce         = WATER_JUMP_FORCE * 1.25f,
            .waterTargetSubmersion  = 0.0f,
            .waterMaxSubmersion     = 1.0f,
            .waterCanJump           = true,
            .waterInfiniteJump      = true,
            .waterAlwaysFloat       = true,
        },
        [PLAYER_TEXTURE_TROLL_FACE] = {
            .gravity        = GRAVITY * 0.5f,
            .moveSpeed      = MOVE_SPEED,
            .jumpForce      = JUMP_FORCE,
            .coyoteTime     = COYOTE_TIME,
            .jumpBufferTime = JUMP_BUFFER_TIME,
            .maxJumps       = MAX_JUMPS,
            .friction       = FRICTION,
            .iceFriction    = ICE_FRICTION,

            .waterBuoyancy          = 0.0f,
            .waterDefaultSink       = -5745.0f,
            .waterSinkWithS         = -8565.0f,
            .waterHorizDrag         = WATER_HORIZ_DRAG * 0.85f,
            .waterVertDrag          = WATER_VERT_DRAG * 0.75f,
            .waterJumpForce         = -2000.0f,
            .waterTargetSubmersion  = 1.0f,
            .waterMaxSubmersion     = 1.0f,
            .waterCanJump           = true,
            .waterInfiniteJump      = false,
            .waterAlwaysFloat       = false,
        },
        [PLAYER_TEXTURE_BATTLESHIP_TODO] = {
            .gravity        = GRAVITY,
            .moveSpeed      = MOVE_SPEED,
            .jumpForce      = 0.0f,
            .coyoteTime     = 0.0f,
            .jumpBufferTime = 0.0f,
            .maxJumps       = 0.0f,
            .friction       = FRICTION,
            .iceFriction    = ICE_FRICTION,

            .waterBuoyancy          = 0.0f,
            .waterDefaultSink       = 0.0f,
            .waterSinkWithS         = 0.0f,
            .waterHorizDrag         = WATER_HORIZ_DRAG * 0.45f,
            .waterVertDrag          = WATER_VERT_DRAG * 0.65f,
            .waterJumpForce         = 0.0f,
            .waterTargetSubmersion  = 0.35,
            .waterMaxSubmersion     = 0.7,
            .waterCanJump           = false,
            .waterInfiniteJump      = false,
            .waterAlwaysFloat       = false,
        },
    };

    // ── Runtime physics constants (live editable via F2 panel) ─────────────
    memcpy(lobby_game.physics, physics, sizeof(physics));

    // Debug panel editing state
    lobby_game.physicsPanelEditIndex  = -1;
    lobby_game.physicsPanelEditBuffer[0] = '\0';
    lobby_game.physicsPanelEditCursor = 0;

    // TODO: To be made as a printError fn
    switch (error) {
        case ERROR_NULL_POINTER: {
            log_error("Received NULL pointer as param");
        } break;

        case ERROR_ALLOC: {
            log_error("Couldn't alloc");
        } break;

        case ERROR_INVALID: {
            log_error("Recieved invalid value");
        } break;

        case ERROR_INVALID_ENUM_VAL: {
            log_error("Recieved invalid enum value");
        } break;

        case ERROR_INVALID_SETTING: {
            log_error("Recieved invalid setting value");
        } break;

        case ERROR_TEXTURE_LOAD: {
            log_error("Couldn't load texture");
        } break;

        case ERROR_AUDIO_LOAD: {
            log_error("Couldn't load audio");
        } break;

        case ERROR_WINDOW_INIT: {
            log_error("Couldn't load initialize window");
        } break;

        case ERROR_ASSET_LOAD: {
            log_error("Couldn't load asset");
        } break;

        case OK: break;

        default: {
            log_error("Unexpected [Error_Et] value: %d", error);
        }
    }
}

/**
 * @brief Handles incoming network data for the lobby module.
 * @param player_id ID of the sender.
 * @param action Action code received.
 * @param data Payload of the data packet.
 * @param len Length of the data payload.
 */
void lobby_on_data(int player_id, u8 action, const void* data, u16 len) {
    if (player_id < 0 || player_id >= MAX_CLIENTS) return;

    switch (action) {
        case ACTION_CODE_LOBBY_MOVE: {
            if (len < sizeof(Player_St)) break;

            memcpy(&lobby_game.otherPlayers[player_id], data, sizeof(Player_St));
            lobby_game.otherPlayers[player_id].active = true;
        } break;

        case ACTION_CODE_LOBBY_CHAT: {
            addChatMessage(TextFormat("Player %d", player_id), (char*)data);
        } break;

        case ACTION_CODE_QUIT_GAME: {
            lobby_game.otherPlayers[player_id].active = false;
        } break;

        default: {
            log_error("Unexpected [ACTION_CODE_Et] value: %d", action);
        }
    }
}

/**
 * @brief Updates the lobby module logic (physics, camera, network sync).
 * @param dt Delta time since the last frame.
 */
void lobby_update(float dt) {
    updateChat();
    
    toggleEditorMode(&lobby_game);

    // If chat is open, we don't move and don't toggle menus
    if (lobby_game.chat.isOpen) {
        // Still update camera to stay centered but no movement
        lobby_game.cam.target = lobby_game.player.position;
        return;
    }

    // Editor takes full control this frame
    // skip normal physics & normal draw
    if (lobby_game.editorMode) {
        updateEditor(&lobby_game, dt);
        return;
    }

    if (IsKeyPressed(KEY_R)) {
        lobby_game.player.position = (f32Vector2) {0};
    }

    if (IsKeyPressed(KEY_F2)) {
        showPhysicsDebugPanel = !showPhysicsDebugPanel;
    }

    if (showPhysicsDebugPanel) {
        updatePhysicsDebugPanel(&lobby_game);

        f32 wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            // Only scroll when mouse is over the panel
            Vector2 mouse = GetMousePosition();
            if (mouse.x > systemSettings.video.width - 400.0f && 
                mouse.y > 20.0f && mouse.y < 460.0f) {
                panelScrollY -= wheel * 35.0f;   // adjust speed to taste
                panelScrollY = clamp(panelScrollY, 0.0f, 800.0f);
            }
        }
    }

    updatePlayer(&lobby_game, dt);
    lobby_game.cam.target = lobby_game.player.position;

    toggleSkinMenu(&lobby_game);
    if (lobby_game.playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&lobby_game);
    }

    bool doSendNewPos = lobby_game.player.position.x != lastSentPos.x 
                     || lobby_game.player.position.y != lastSentPos.y 
                     || firstFrame;

    if (doSendNewPos) {
        GameTLVHeader_St tlv = {
            .game_id = MINI_GAME_LOBBY, 
            .action = ACTION_CODE_LOBBY_MOVE, 
            .length = sizeof(Player_St)
        };

        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, 5, &h);
        
        u8 buffer[1024];
        size_t offset = 0;
        
        memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
        memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
        memcpy(buffer + offset, &lobby_game.player, sizeof(Player_St)); offset += sizeof(Player_St);
        
        send(networkSocket, buffer, offset, 0);
        lastSentPos = lobby_game.player.position;
        firstFrame = false;
    }
}

/**
 * @brief Renders the lobby module (platforms, players, UI).
 */
void lobby_draw(void) {
    BeginMode2D(lobby_game.cam); {
        drawLobbyTerrains();
        drawPlayer(&lobby_game.playerVisuals, &lobby_game.player);
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!lobby_game.otherPlayers[i].active) continue;
            drawPlayer(&lobby_game.playerVisuals, &lobby_game.otherPlayers[i]);
        }
    } EndMode2D();

    if (lobby_game.editorMode) {
        drawEditor(&lobby_game);
        return;
    }

    static f32 lobbyTextXPos;
    lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
    DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

    drawSkinButton();
    if (lobby_game.playerVisuals.isTextureMenuOpen) {
        drawMenuTextures(&lobby_game);
    }

    drawPhysicsDebugPanel(&lobby_game);

    drawChat();
}

/** @brief Global definition of the Lobby module. */
GameClientInterface_St lobbyClientInterface = {
    .id         = MINI_GAME_LOBBY,
    .name       = "Lobby",
    .init       = lobby_init,
    .on_data    = lobby_on_data,
    .update     = lobby_update,
    .draw       = lobby_draw
};