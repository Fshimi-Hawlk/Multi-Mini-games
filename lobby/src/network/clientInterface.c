/**
    @file clientInterface.c
    @author i-Charlys (CAILLON Charles)
    @author Fshimi-Hawlk
    @date 2026-03-18
    @date 2026-03-27
    @brief Implementation of the Lobby mini-game module.
*/

#include "core/game.h"
#include "core/chat.h"
#include "ui/game.h"
#include "ui/app.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "systemSettings.h"
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

static f32Vector2 lastSentPos = {0};
static bool firstFrame = true;

void lobby_init(void) {
    log_debug("[LOBBY]: Initializing client lobby");
    
    memset(&lobby_game, 0, sizeof(lobby_game));

    systemSettings.video.height = DEFAULT_VIDEO_SETTING_HEIGHT;
    systemSettings.video.width = DEFAULT_VIDEO_SETTING_WIDTH;
    applySystemSettings();

    lobby_game.player = (Player_St) {
        .position           = {0, 0},
        .radius             = 20,
        .coyoteTime         = 0.1f,
        .coyoteTimer        = 0.1f,
        .active             = true,
        .unlockedTextures   = {
            [PLAYER_TEXTURE_DEFAULT] = true,
            [PLAYER_TEXTURE_EARTH] = true,
            [PLAYER_TEXTURE_TROLL_FACE] = true,
            [PLAYER_TEXTURE_KFF] = true,
            [PLAYER_TEXTURE_BINGO] = true,
        },
    };
    strncpy(lobby_game.player.name, "Moi", 31);

    lobby_game.cam = (Camera2D) {
        .offset = {systemSettings.video.width / 2.0f, systemSettings.video.height / 2.0f},
        .zoom   = 1.0f,
    };
    
    lobby_game.playerVisuals.defaultTextureRect = (Rectangle) {
        .x = 20, .y = 60, .width = 50, .height = 50
    };

    const char* playerTextureImagePaths[__playerTextureCount] = {
        [PLAYER_TEXTURE_EARTH]      = IMAGES_PATH "earth.png",
        [PLAYER_TEXTURE_TROLL_FACE] = IMAGES_PATH "trollFace.png",
        [PLAYER_TEXTURE_BINGO]      = IMAGES_PATH "bingo69.png",
        [PLAYER_TEXTURE_KFF]        = IMAGES_PATH "king67.png",
    };

    for (u8 i = 0; i < __playerTextureCount; ++i) {
        if (playerTextureImagePaths[i]) {
            lobby_game.playerVisuals.textures[i] = LoadTexture(playerTextureImagePaths[i]);
        }
    }
    
    logoSkinButton = LoadTexture(IMAGES_PATH "logoSkin.png");
    lobby_game.currentState = GAME_STATE_CONNECTION;
}

void lobby_on_data(int playerID, u8 action, const void* data, u16 len) {
    if (playerID < 0 || playerID >= MAX_CLIENTS) return;
    if (playerID == lobby_game.id) return;

    switch (action) {
        case ACTION_CODE_JOIN_ACK: {
            u16 tempID;
            memcpy(&tempID, data, sizeof(u16)); 
            lobby_game.id = ntohs(tempID);
            log_info("[LOBBY] My network ID is now %d", lobby_game.id);
        } break;

        case ACTION_CODE_LOBBY_MOVE: {
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
            addChatMessage(TextFormat("Joueur %d", playerID), (char*)data);
        } break;

        case ACTION_CODE_QUIT_GAME: {
            lobby_game.otherPlayers[playerID].active = false;
        } break;
    }
}

void lobby_update(float dt) {
    if (lobby_game.chat.isOpen) {
        lobby_game.cam.target = lobby_game.player.position;
        return;
    }

    if (IsKeyPressed(KEY_R)) lobby_game.player.position = (Vector2) {0, 0};

    updatePlayer(&lobby_game.player, dt);
    lobby_game.cam.target = lobby_game.player.position;

    toggleSkinMenu(&lobby_game.playerVisuals);
    if (lobby_game.playerVisuals.isTextureMenuOpen) {
        choosePlayerTexture(&lobby_game.player, &lobby_game.playerVisuals);
    }

    if (lobby_game.player.position.x != lastSentPos.x || lobby_game.player.position.y != lastSentPos.y || firstFrame) {
        PlayerNet_St net = {
            .x = lobby_game.player.position.x,
            .y = lobby_game.player.position.y,
            .angle = lobby_game.player.angle,
            .textureId = (u8)lobby_game.player.textureId,
            .active = true
        };
        strncpy(net.name, lobby_game.player.name, 31);

        GameTLVHeader_St tlv = { .game_id = MINI_GAME_LOBBY, .action = ACTION_CODE_LOBBY_MOVE, .length = sizeof(PlayerNet_St) };
        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
        
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
        drawLobbyTerrains(terrains, gameInteractionZones);
        drawPlayer(&lobby_game.playerVisuals, &lobby_game.player);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (lobby_game.otherPlayers[i].active)
                drawPlayer(&lobby_game.playerVisuals, &lobby_game.otherPlayers[i]);
        }
    } EndMode2D();
    
    DrawText("Multi-Mini-Games", (GetScreenWidth() - MeasureText("Multi-Mini-Games", 20)) / 2, 20, 20, PURPLE);
    drawSkinButton();
    if (lobby_game.playerVisuals.isTextureMenuOpen) drawMenuTextures(&lobby_game);
    drawChat();
}

GameClientInterface_St lobbyClientInterface = {
    .id         = MINI_GAME_LOBBY,
    .name       = "Lobby",
    .init       = lobby_init,
    .on_data    = lobby_on_data,
    .update     = lobby_update,
    .draw       = lobby_draw
};
