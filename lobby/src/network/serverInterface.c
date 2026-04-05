/**
    @file network/serverInterface.c
    @author i-Charlys (CAILLON Charles)
    @date 2026-03-18
    @date 2026-04-04
    @brief Server-side logic for the lobby module.

    Handles player movement synchronization, join notifications, and cleanup when players leave.
    All game-specific actions are simply broadcast to the room.

    Contributors:
        - i-Charlys (CAILLON Charles):
            - Original implementation
        - Fshimi-Hawlk:
            - Full CSC compliance, English translation
*/

#include "utils/userTypes.h"

#include "utils/debug.h"

#include "networkInterface.h"

/**
    @brief Allocates and initializes a new lobby game instance.
    @return Pointer to a zero-initialized LobbyGame_St, or NULL on allocation failure.
*/
void* lobby_createInstance(void) {
    return calloc(1, sizeof(LobbyGame_St));
}

/**
    @brief Handles an incoming player action for the lobby instance.

    @param instance     Owning GameInstance_St (use instance->gameState for LobbyGame_St).
    @param playerId     Player who sent the action.
    @param action       Action code.
    @param payload      Payload data.
    @param len          Payload length.
    @param broadcast    Scoped broadcast function (must be called with the same instance).
*/
void lobby_onAction(GameInstance_St* instance, s32 playerID, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    LobbyGame_St* game = (LobbyGame_St*) instance->gameState;

    if (action == ACTION_CODE_JOIN_GAME) {
        // Send all currently active players to the new client
        for (s32 i = 0; i < MAX_CLIENTS; ++i) {
            if (i != playerID && game->otherPlayers[i].active) {
                broadcast(instance, i, ACTION_CODE_LOBBY_MOVE, &game->otherPlayers[i], sizeof(Player_St));
            }
        }
    } else if (action == ACTION_CODE_LOBBY_MOVE) {
        if (playerID >= 0 && playerID < MAX_CLIENTS && len == sizeof(Player_St)) {
            memcpy(&game->otherPlayers[playerID], payload, sizeof(Player_St));
            game->otherPlayers[playerID].active = true;
        }
        // Relay the move to everyone (including the sender for confirmation)
        broadcast(instance, BROADCAST_ALL, action, payload, len);
    } else {
        // Any other action is simply broadcast to the room
        broadcast(instance, BROADCAST_ALL, action, payload, len);
    }
}

/**
    @brief Cleans up a player's data when they leave the lobby.

    Called by the server when a client disconnects or times out.
    Does **not** perform any game switching — that is handled in server.c.

    @param state        Pointer to the LobbyGame_St instance.
    @param playerID     ID of the player who left.
*/
void lobby_onPlayerLeave(void* state, s32 playerID) {
    LobbyGame_St* game = (LobbyGame_St*) state;

    if (playerID >= 0 && playerID < MAX_CLIENTS) {
        memset(&game->otherPlayers[playerID], 0, sizeof(Player_St));
    }

    // Count remaining active players and log (for easier debugging, matching Bingo style)
    s32 remaining = 0;
    for (s32 i = 0; i < MAX_CLIENTS; ++i) {
        if (game->otherPlayers[i].active) {
            ++remaining;
        }
    }

    log_info("Player %d left the lobby (remaining: %d)", playerID, remaining);
}

/**
    @brief Server-side tick for the lobby (currently does nothing).
    @param state Pointer to the LobbyGame_St instance.
*/
void lobby_onTick(void* state) {
    UNUSED(state);
}

/**
    @brief Destroys a lobby instance and frees its memory.
    @param state Pointer previously returned by lobby_createInstance().
*/
void lobby_destroyInstance(void* state) {
    if (state) {
        free(state);
    }
}

/**
    @brief Public interface exposed to the server.
*/
GameServerInterface_St lobbyServerInterface = {
    .game_name          = "lobby",
    .createInstance    = lobby_createInstance,
    .onAction          = lobby_onAction,
    .onTick            = lobby_onTick,
    .destroyInstance   = lobby_destroyInstance,
    .onPlayerLeave    = lobby_onPlayerLeave
};