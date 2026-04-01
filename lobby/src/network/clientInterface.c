/**
    @file network/clientInterface.c
    @author i-Charlys
    @author Fshimi-Hawlk
    @date 2026-03-18
    @brief Implementation of the Lobby mini-game client module (networking and scene bridge).
*/

#include "core/game.h"       
#include "core/chat.h"

#include "ui/game.h"
#include "ui/app.h"

#include "utils/globals.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

extern int my_id;
#include "lobbyAPI.h"
extern LobbyGame_St* g_lobbyGame;

/** @brief Last sent player position to avoid redundant network updates. */
static Vector2 lastSentPos = {0};
/** @brief Flag for the first frame update. */
static bool firstFrame = true;
/** @brief Timer to force a network sync even if stationary. */
static float syncTimer = 0;

/**
 * @brief Handles incoming network data for the lobby module.
 * @param player_id ID of the sender.
 * @param action Action code received.
 * @param data Payload of the data packet.
 * @param len Length of the data payload.
 */
void lobby_on_data(int player_id, u8 action, const void* data, u16 len) {
    if (player_id >= 0 && player_id < MAX_CLIENTS) {
        // Skip if this is our own data mirrored back
        if (player_id == my_id) return;

        LobbyGame_St* game = g_lobbyGame;
        if (!game) return;
        
        if (action == ACTION_CODE_LOBBY_MOVE && len >= sizeof(PlayerNet_St)) {
            PlayerNet_St net;
            memcpy(&net, data, sizeof(PlayerNet_St));
            
            if (!game->otherPlayers[player_id].active) {
                printf("[LOBBY] Nouveau joueur détecté : ID %d\n", player_id);
                game->otherPlayers[player_id].position = (Vector2){ net.x, net.y };
                game->otherPlayers[player_id].targetPosition = (Vector2){ net.x, net.y };
            }

            game->otherPlayers[player_id].targetPosition = (Vector2){ net.x, net.y };
            game->otherPlayers[player_id].angle    = net.angle;
            game->otherPlayers[player_id].textureId = net.textureId;
            strncpy(game->otherPlayers[player_id].name, net.name, 31);
            game->otherPlayers[player_id].name[31] = '\0';
            game->otherPlayers[player_id].active    = true;
            game->otherPlayers[player_id].radius    = 20.0f; // Force radius for visibility
        }
        else if (action == ACTION_CODE_LOBBY_CHAT) {
            addChatMessage(TextFormat("Player %d", player_id), (char*)data);
        }
        else if (action == ACTION_CODE_QUIT_GAME) {
            game->otherPlayers[player_id].active = false;
        }
    }
}

/**
 * @brief Updates the lobby module logic (network sync).
 * @param dt Delta time since the last frame.
 */
void lobby_update(float dt) {
    LobbyGame_St* game = g_lobbyGame;
    if (!game) return;
    
    // updateChat() is called in main.c
    
    syncTimer += dt;
    // We sync more often (0.5s) to ensure visibility for new joiners
    bool shouldSync = (game->player.position.x != lastSentPos.x || game->player.position.y != lastSentPos.y || firstFrame || syncTimer > 0.5f);

    if (shouldSync) {
        PlayerNet_St net = {
            .x = game->player.position.x,
            .y = game->player.position.y,
            .angle = game->player.angle,
            .textureId = (u8)game->player.textureId,
            .active = true
        };
        strncpy(net.name, game->player.name, 31);
        net.name[31] = '\0';

        GameTLVHeader_St tlv = { .game_id = 0, .action = ACTION_CODE_LOBBY_MOVE, .length = sizeof(PlayerNet_St) };
        RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_GAME_DATA, &h);
        
        u8 buffer[1024];
        size_t offset = 0;
        
        memcpy(buffer + offset, &h, sizeof(h)); offset += sizeof(h);
        memcpy(buffer + offset, &tlv, sizeof(tlv)); offset += sizeof(tlv);
        memcpy(buffer + offset, &net, sizeof(PlayerNet_St)); offset += sizeof(PlayerNet_St);
        
        send(networkSocket, buffer, offset, 0);
        lastSentPos = game->player.position;
        firstFrame = false;
        syncTimer = 0;
    }
}

/**
 * @brief Renders the lobby module (already handled in lobbyAPI.c for now, but keeping for interface).
 */
void lobby_draw(void) {
    // Handled in lobbyAPI.c
}

/** @brief Global definition of the Lobby module interface. */
GameClientInterface_St LobbyModule = {
    .id = 0,
    .name = "Lobby Principal",
    .init = NULL, // Handled in lobbyAPI.c
    .on_data = lobby_on_data,
    .update = lobby_update,
    .draw = lobby_draw
};
