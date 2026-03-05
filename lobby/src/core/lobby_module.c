/**
 * @file lobby_module.c
 * @brief Le VRAI module du Lobby : Physique, Caméra, Réseau et Fix Pointer !
 */

#include "firstparty/APIs/module_interface.h"
#include "core/game.h"       
#include "ui/game.h"         
#include "ui/app.h"          
#include "utils/globals.h"   
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

static Vector2 lastSentPos = {0};
static Camera2D camera = { 0 };

void lobby_init(void) {
    printf("[LOBBY] Chargement du niveau et de la caméra...\n");
    
    // Chemins en dur pour esquiver les bugs du Makefile
    playerTextures[0] = LoadTexture("lobby/assets/trollFace.png");
    playerTextures[1] = LoadTexture("lobby/assets/earth.png");
    playerTextureCount = 2;
    logoSkinButton = LoadTexture("lobby/assets/logoSkin.png");
    
    player.texture = &playerTextures[0];

    // Initialisation de la Caméra pour voir le monde
    camera.target = player.position;
    camera.offset = (Vector2){ 1280.0f / 2.0f, 720.0f / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; 
}

void lobby_on_data(int player_id, uint8_t action, void* data, uint16_t len) {
    if (player_id >= 0 && player_id < MAX_CLIENTS && action == 2 /* LOBBY_MOVE */) {
        if (len >= sizeof(Player_st)) {
            memcpy(&otherPlayers[player_id], data, sizeof(Player_st));
            otherPlayers[player_id].active = true;
            
            otherPlayers[player_id].texture = &playerTextures[0]; 
        }
    }
}

void lobby_update(float dt) {
    // 1. Physique
    updatePlayer(&player, platforms, platformCount, dt);

    // 2. La caméra suit le joueur
    camera.target = player.position;

    // 3. UI Skin
    toggleSkinMenu();
    if (isTextureMenuOpen) {
        choosePlayerTexture(&player);
    }

    // 4. Réseau : Envoi du TLV
    if (player.position.x != lastSentPos.x || player.position.y != lastSentPos.y) {
        GameTLVHeader tlv = { .game_id = 0, .action = 2, .length = sizeof(Player_st) };
        RUDP_Header h; RUDP_GenerateHeader(&server_conn, 5, &h);
        
        uint8_t buffer[1024];
        size_t offset = 0;
        
        memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
        memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
        memcpy(buffer + offset, &player, sizeof(Player_st)); offset += sizeof(Player_st);
        
        send(network_socket, buffer, offset, 0);
        lastSentPos = player.position;
    }
}

void lobby_draw(void) {
    // --- MODE MONDE (Ce qui bouge avec la caméra) ---
    BeginMode2D(camera);
        drawPlatforms(platforms, platformCount);
        drawPlayer(&player);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (otherPlayers[i].active) drawPlayer(&otherPlayers[i]);
        }
    EndMode2D();
    
    // --- MODE UI (Ce qui est collé à l'écran) ---
    drawSkinButton();
    if (isTextureMenuOpen) {
        drawMenuTextures();
    }
}

MiniGameModule LobbyModule = {
    .id = 0,
    .name = "Lobby Principal",
    .init = lobby_init,
    .on_data = lobby_on_data,
    .update = lobby_update,
    .draw = lobby_draw
};