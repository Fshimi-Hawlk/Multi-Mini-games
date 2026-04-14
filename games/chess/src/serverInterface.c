#include "chessAPI.h"

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#pragma pack(push, 1)
typedef struct {
    u8 from_x, from_y;
    u8 to_x, to_y;
    u8 promotion;
} ChessMovePayload_St;
#pragma pack(pop)

typedef struct {
    int players[2]; // IDs of white and black players
    int numPlayers;
    int turn; // 0 or 1
    // Board state could be here too if we wanted authoritative server
} ChessServerState;

void* chess_createInstance(void) {
    ChessServerState* cs = calloc(1, sizeof(ChessServerState));
    if (cs) {
        cs->players[0] = -1;
        cs->players[1] = -1;
        cs->numPlayers = 0;
        cs->turn = 0;
    }
    return cs;
}

void chess_onAction(void *state, s32 roomId, s32 playerId, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA) return;
    if (len < sizeof(GameTLVHeader_St)) return;
    
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->gameId != MINI_GAME_ID_CHESS) return;
    
    ChessServerState* cs = (ChessServerState*)state;
    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);
    (void)realPayload;

    if (realAction == ACTION_CODE_JOIN_GAME) {
        int internalId = -1;
        // Assign white then black
        if (cs->players[0] == -1) {
            cs->players[0] = playerId;
            internalId = 0;
            cs->numPlayers++;
        } else if (cs->players[1] == -1 && cs->players[0] != playerId) {
            cs->players[1] = playerId;
            internalId = 1;
            cs->numPlayers++;
        }
        
        if (internalId != -1) {
            u8 bufAck[64];
            memset(bufAck, 0, sizeof(bufAck));
            GameTLVHeader_St tlvAck = { .gameId = MINI_GAME_ID_CHESS, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
            u16 netInternalId = htons((u16)internalId);
            memcpy(bufAck, &tlvAck, sizeof(tlvAck));
            memcpy(bufAck + sizeof(tlvAck), &netInternalId, sizeof(u16));
            broadcast(UNICAST, playerId, ACTION_CODE_GAME_DATA, bufAck, sizeof(tlvAck) + sizeof(u16));
        }
    }
    else if (realAction == ACTION_CODE_START_GAME) {
        // Broadcast game start to everyone else
        broadcast(roomId, playerId, ACTION_CODE_GAME_DATA, payload, len);
        // Also send back to host so they start the game too
        broadcast(UNICAST, playerId, ACTION_CODE_GAME_DATA, payload, len);
    }
    else if (realAction == ACTION_CODE_CHESS_MOVE) {
        // Broadcast move to all (including sender for simplicity or excluding)
        // In our case, we broadcast to everyone so the other player sees it.
        broadcast(roomId, playerId, ACTION_CODE_GAME_DATA, payload, len);
        cs->turn = !cs->turn;
    }
}

void chess_onTick(void* state) {
    (void)state;
}

void chess_onPlayerLeave(void* state, s32 playerId) {
    ChessServerState* cs = (ChessServerState*)state;
    if (cs->players[0] == playerId) { cs->players[0] = -1; cs->numPlayers--; }
    if (cs->players[1] == playerId) { cs->players[1] = -1; cs->numPlayers--; }
}

void chess_destroyInstance(void *state) {
    free(state);
}

GameServerInterface_St chessServerInterface = {
    .gameName = "chess",
    .createInstance = chess_createInstance,
    .onAction = chess_onAction,
    .onTick = chess_onTick,
    .onPlayerLeave = chess_onPlayerLeave,
    .destroyInstance = chess_destroyInstance
};
