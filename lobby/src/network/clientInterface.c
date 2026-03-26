/**
 * @file lobby_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of the Lobby mini-game module.
 */

#include "core/game.h"
#include "core/chat.h"

#include "networkInterface.h"
#include "ui/game.h"
#include "ui/app.h"

#include "utils/globals.h"

#include "systemSettings.h"
#include "lobbyAPI.h"
#include "APIs/generalAPI.h"

LobbyGame_St lobby_game = {0};

f32Vector2 lastSentPos = {0};
bool firstFrame = true;

/**
 * @brief Initializes the lobby module (loads textures, sets camera).
 */
void lobby_init(void) {
    log_debug("[LOBBY]: Initializing client lobby");
    
    Error_Et error;
    memset(&lobby_game, 0, sizeof(lobby_game));

    systemSettings.video.height = DEFAULT_VIDEO_SETTING_HEIGHT;
    systemSettings.video.width = DEFAULT_VIDEO_SETTING_WIDTH;
    applySystemSettings();

    /** Hitbox that triggers the Tetris mini-lobby_game when player collides */
    Rectangle gameHitboxes[__miniGameCount] = { 
        [MINI_GAME_BINGO] = {
            .x      = 600,
            .y      = -150,
            .width  = 75,
            .height = 75
        },
    };

    memcpy(lobby_game.miniGameManager.gameHitboxes, gameHitboxes, sizeof(gameHitboxes));

    /** Current active scene (lobby or one of the mini-games) */
    lobby_game.miniGameManager.currentMiniGame = MINI_GAME_LOBBY;
    
    /** Player controlled by the user in the lobby */
    lobby_game.player = (Player_St) {
        .position   = {0, 250},
        .radius     = 20,
        .coyoteTime = 0.1f,
        .coyoteTimer= 0.1f,
        .jumpBuffer = 0.2f
    };

    lobby_game.player.unlockedTextures[PLAYER_TEXTURE_DEFAULT] = 1;
    lobby_game.player.unlockedTextures[PLAYER_TEXTURE_EARTH] = 1;

    /** Camera following the player in 2D mode */
    lobby_game.cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };
    
    lobby_game.playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    // Load shared UI textures
    lobby_game.playerVisuals.textures[PLAYER_TEXTURE_EARTH] = LoadTexture(IMAGES_PATH "earth.png");
    if (!IsTextureValid(lobby_game.playerVisuals.textures[PLAYER_TEXTURE_EARTH])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "earth.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    lobby_game.playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE] = LoadTexture(IMAGES_PATH "trollFace.png");
    if (!IsTextureValid(lobby_game.playerVisuals.textures[PLAYER_TEXTURE_TROLL_FACE])) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "trollFace.png");
        error =  ERROR_TEXTURE_LOAD;
    }
    
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    if (!IsTextureValid(logoSkinButton)) {
        log_warn("%s couldn't be loaded proprely.", IMAGES_PATH "logoSkin.png");
        error =  ERROR_TEXTURE_LOAD;
    }

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
    
    // If chat is open, we don't move and don't toggle menus
    if (lobby_game.chat.isOpen) {
        // Still update camera to stay centered but no movement
        lobby_game.cam.target = lobby_game.player.position;
        return;
    }

    updatePlayer(&lobby_game.player, platforms, platformCount, dt);
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
    static f32 lobbyTextXPos;

    BeginMode2D(lobby_game.cam);
        drawPlatforms(platforms, platformCount);
        drawPlayer(&lobby_game.playerVisuals, &lobby_game.player);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!lobby_game.otherPlayers[i].active) continue;
            drawPlayer(&lobby_game.playerVisuals, &lobby_game.otherPlayers[i]);
        }

        for (u8 i = 1; i < __miniGameCount; ++i) {
            DrawRectangleRec(lobby_game.miniGameManager.gameHitboxes[i], RED); // Debug hitbox
        }
    EndMode2D();
    
    lobbyTextXPos = (systemSettings.video.width - MeasureText("Multi-Mini-Games", 20)) / 2.0f;
    DrawText("Multi-Mini-Games", lobbyTextXPos, 20, 20, PURPLE);

    drawSkinButton();
    if (lobby_game.playerVisuals.isTextureMenuOpen) {
        drawMenuTextures(&lobby_game);
    }

    drawChat();
}

/** @brief Global definition of the Lobby module. */
GameClientInterface_St LobbyModule = {
    .id         = MINI_GAME_LOBBY,
    .name       = "Lobby",
    .init       = lobby_init,
    .on_data    = lobby_on_data,
    .update     = lobby_update,
    .draw       = lobby_draw
};