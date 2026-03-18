/**
 * @file lobby_module.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of the Lobby mini-game module.
 */

#include "firstparty/APIs/module_interface.h"
#include "core/game.h"       
#include "ui/game.h"         
#include "ui/app.h"          
#include "utils/globals.h"
#include "utils/chat.h"
#include "rudp_core.h"
#include "raylib.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

extern int network_socket;
extern RUDP_Connection server_conn;
extern Player_st player;
extern Player_st otherPlayers[MAX_CLIENTS];
extern int my_id;

/** @brief Last sent player position to avoid redundant network updates. */
static Vector2 lastSentPos = {0};
/** @brief Camera used to follow the player in the lobby. */
static Camera2D camera = { 0 };
/** @brief Flag for the first frame update. */
static bool firstFrame = true;
/** @brief Timer to force a network sync even if stationary. */
static float syncTimer = 0;

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
    syncTimer = 0;
}

/**
 * @brief Handles incoming network data for the lobby module.
 * @param player_id ID of the sender.
 * @param action Action code received.
 * @param data Payload of the data packet.
 * @param len Length of the data payload.
 */
void lobby_on_data(int player_id, uint8_t action, void* data, uint16_t len) {
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        // Skip if this is our own data mirrored back
        if (player_id == my_id) return;

        if (action == 2 /* LOBBY_MOVE */ && len >= sizeof(PlayerNet_st)) {
            PlayerNet_st net;
            memcpy(&net, data, sizeof(PlayerNet_st));
            
            if (!otherPlayers[player_id].active) {
                printf("[LOBBY] Nouveau joueur détecté : ID %d\n", player_id);
            }

            otherPlayers[player_id].position = (Vector2){ net.x, net.y };
            otherPlayers[player_id].angle = net.angle;
            otherPlayers[player_id].skin_id = net.skin_id;
            otherPlayers[player_id].active = true;
            otherPlayers[player_id].radius = 20.0f; // Force radius for visibility
            
            // Assign texture based on synced skin_id
            int sid = otherPlayers[player_id].skin_id;
            if (sid >= 0 && sid < playerTextureCount) {
                otherPlayers[player_id].texture = &playerTextures[sid];
            } else {
                otherPlayers[player_id].texture = &playerTextures[0];
            }
        }
        else if (action == 5 /* LOBBY_CHAT */) {
            AddChatMessage(TextFormat("Player %d", player_id), (char*)data);
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
    UpdateChat();
    
    // If chat is open, we don't move and don't toggle menus
    if (g_chatState.isOpen) {
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

    syncTimer += dt;
    // We sync more often (0.5s) to ensure visibility for new joiners
    bool shouldSync = (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y || firstFrame || syncTimer > 0.5f);

    if (shouldSync) {
        PlayerNet_st net = {
            .x = player.position.x,
            .y = player.position.y,
            .angle = player.angle,
            .skin_id = player.skin_id,
            .active = true
        };

        GameTLVHeader tlv = { .game_id = 0, .action = 2, .length = sizeof(PlayerNet_st) };
        RUDP_Header h; RUDP_GenerateHeader(&server_conn, ACTION_GAME_DATA, &h);
        
        uint8_t buffer[1024];
        size_t offset = 0;
        
        memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
        memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
        memcpy(buffer + offset, &net, sizeof(PlayerNet_st)); offset += sizeof(PlayerNet_st);
        
        send(network_socket, buffer, offset, 0);
        lastSentPos = player.position;
        firstFrame = false;
        syncTimer = 0;
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
            if (otherPlayers[i].active && i != my_id) drawPlayer(&otherPlayers[i]);
        }
    EndMode2D();
    
    drawSkinButton();
    if (isTextureMenuOpen) {
        drawMenuTextures();
    }
    DrawChat();
}

/** @brief Global definition of the Lobby module. */
MiniGameModule LobbyModule = {
    .id = 0,
    .name = "Lobby Principal",
    .init = lobby_init,
    .on_data = lobby_on_data,
    .update = lobby_update,
    .draw = lobby_draw
};
