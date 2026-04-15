/**
    @file serverInterface.c
    @author Léandre BAUDET
    @date 2026-03-30
    @date 2026-04-14
    @brief Server-side implementation of the Lobby module.
*/

#include "networkInterface.h"
#include "APIs/generalAPI.h"

#include "sharedUtils/debug.h"

typedef struct {
    int dummy;
} LobbyServerState;

void* lobby_createInstance(void) {
    LobbyServerState* s = malloc(sizeof(LobbyServerState));
    if (s) s->dummy = 0;
    return s;
}

void lobby_onAction(void* state, s32 room_id, s32 player_id, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    UNUSED(state);
    if (action == ACTION_CODE_GAME_DATA && len >= sizeof(GameTLVHeader_St)) {
        GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
        if (tlv->gameId == MINI_GAME_ID_LOBBY) {
            if (tlv->action == ACTION_CODE_LOBBY_MOVE || tlv->action == ACTION_CODE_LOBBY_CHAT) {
                // Broadcast movement and chat to everyone
                broadcast(room_id, player_id, action, payload, len);
            }
        }
    }
}

void lobby_tick(void* state) {
    UNUSED(state);
}

void lobby_onPlayerLeave(void* state, s32 player_id) {
    UNUSED(state);
    UNUSED(player_id);
}

void lobby_destroyInstance(void* state) {
    if (state) free(state);
}

GameServerInterface_St lobby_serverInterface = {
    .gameName = "lobby",
    .createInstance = lobby_createInstance,
    .onAction = lobby_onAction,
    .onTick = lobby_tick,
    .onPlayerLeave = lobby_onPlayerLeave,
    .destroyInstance = lobby_destroyInstance
};
