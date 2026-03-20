/**
 * @file lobby_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of the Lobby mini-game module.
 */

#include "core/game.h"       
#include "core/chat.h"

#include "ui/game.h"
#include "ui/app.h"

#include "utils/globals.h"

extern int networkSocket;
extern RUDPConnection_St serverConnection;
extern Player_St player;
extern Player_St otherPlayers[MAX_CLIENTS];

/** @brief Last sent player position to avoid redundant network updates. */
static Vector2 lastSentPos = {0};
/** @brief Camera used to follow the player in the lobby. */
static Camera2D camera = { 0 };
/** @brief Flag for the first frame update. */
static bool firstFrame = true;

/**
 * @brief Initializes the lobby module (loads textures, sets camera).
 */
void lobby_init(void) {
    printf("[LOBBY] Chargement du niveau et de la caméra...\n");
    
    playerTextures[0] = LoadTexture("assets/images/trollFace.png");
    if (playerTextures[0].id == 0) playerTextures[0] = LoadTexture("lobby/assets/images/trollFace.png");

    playerTextures[1] = LoadTexture("assets/images/earth.png");
    if (playerTextures[1].id == 0) playerTextures[1] = LoadTexture("lobby/assets/images/earth.png");

    playerTextureCount = 2;
    logoSkinButton = LoadTexture("assets/images/logoSkin.png");
    if (logoSkinButton.id == 0) logoSkinButton = LoadTexture("lobby/assets/images/logoSkin.png");
    
    player.texture = &playerTextures[0];

    camera.target = player.position;
    camera.offset = (Vector2){ 1280.0f / 2.0f, 720.0f / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; 
    firstFrame = true;
}

/**
 * @brief Handles incoming network data for the lobby module.
 * @param player_id ID of the sender.
 * @param action Action code received.
 * @param data Payload of the data packet.
 * @param len Length of the data payload.
 */
void lobby_on_data(int player_id, u8 action, void* data, u16 len) {
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        if (action == 2 /* LOBBY_MOVE */ && len >= sizeof(Player_St)) {
            memcpy(&otherPlayers[player_id], data, sizeof(Player_St));
            otherPlayers[player_id].active = true;
            otherPlayers[player_id].texture = &playerTextures[0]; 
        }
        else if (action == 5 /* LOBBY_CHAT */) {
            addChatMessage(TextFormat("Player %d", player_id), (char*)data);
        }
        else if (action == 6 /* LOBBY_LEAVE */) {
            otherPlayers[player_id].active = false;
        }
    }
}

/**
 * @brief Updates the lobby module logic (physics, camera, network sync).
 * @param dt Delta time since the last frame.
 */
void lobby_update(float dt) {
    bool chatWasOpen = gameChat.isOpen;
    updateChat();
    
    // If chat is open, we don't move and don't toggle menus
    if (gameChat.isOpen) {
        // Still update camera to stay centered but no movement
        camera.target = player.position;
        return;
    }

    updatePlayer(&player, platforms, platformCount, dt);
    camera.target = player.position;

    toggleSkinMenu();
    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y || firstFrame) {
        GameTLVHeader_St tlv = { .game_id = 0, .action = 2, .length = sizeof(Player_St) };
        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, 5, &h);
        
        u8 buffer[1024];
        size_t offset = 0;
        
        memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
        memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
        memcpy(buffer + offset, &player, sizeof(Player_St)); offset += sizeof(Player_St);
        
        send(networkSocket, buffer, offset, 0);
        lastSentPos = player.position;
        firstFrame = false;
    }
}

/**
 * @brief Renders the lobby module (platforms, players, UI).
 */
void lobby_draw(void) {
    BeginMode2D(camera);
        drawPlatforms(platforms, platformCount);
        drawPlayer(&player);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (otherPlayers[i].active) drawPlayer(&otherPlayers[i]);
        }
    EndMode2D();
    
    drawSkinButton();
    if (isTextureMenuOpen) {
        drawMenuTextures();
    }
    DrawChat();
}

/** @brief Global definition of the Lobby module. */
GameClientInterface_St LobbyModule = {
    .id = 0,
    .name = "Lobby Principal",
    .init = lobby_init,
    .on_data = lobby_on_data,
    .update = lobby_update,
    .draw = lobby_draw
};