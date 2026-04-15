/**
    @file serverInterface.c
    @author i-Charlys
    @date 2026-04-02
    @date 2026-04-14
    @brief Server-side implementation of the Twist Cube game interface handling multiplayer logic.
*/

#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>


#include "APIs/generalAPI.h"
#include "networkInterface.h"

/**
    @brief Definition of enum RubikActionCodes_e
*/
enum RubikActionCodes_e {
    ACTION_CODE_RUBIK_SCRAMBLE = firstAvailableActionCode + 0x20,
    ACTION_CODE_RUBIK_PROGRESS,
    ACTION_CODE_RUBIK_ELIMINATE
};

/**
    @brief Definition of typedef struct
*/
typedef struct {
    BaseGame_St base;
} RubikGame_St;

typedef struct {
    int id;
    float progress;
    bool active;
    bool eliminated;
} RubikPlayer;

typedef struct {
    RubikPlayer players[MAX_CLIENTS];
    int status; // 0: WAITING, 1: PLAYING
    float eliminationTimer;
    int seed;
} RubikServerState;

void* twistCube_createInstance(void) {
    RubikServerState* rs = calloc(1, sizeof(RubikServerState));
    if (rs) {
        rs->status = 0;
        rs->seed = (int)time(NULL);
        rs->eliminationTimer = 0;
    }
    return rs;
}

void twistCube_onAction(void *state, s32 roomId, s32 playerId, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA) return;
    if (len < sizeof(GameTLVHeader_St)) return;
    
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->gameId != MINI_GAME_ID_TWIST_CUBE) return;
    
    RubikServerState* rs = (RubikServerState*)state;
    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);
    (void)realPayload;

    if (realAction == ACTION_CODE_JOIN_GAME) {
        if (playerId < 0 || playerId >= MAX_CLIENTS) return;
        rs->players[playerId].active = true;
        rs->players[playerId].id = playerId;
        rs->players[playerId].progress = 0;
        rs->players[playerId].eliminated = false;
        
        int internalId = playerId; // Simple mapping
        u16 netId = htons((u16)internalId);
        u8 bufAck[64];
        memset(bufAck, 0, sizeof(bufAck));
        GameTLVHeader_St tlvAck = { .gameId = MINI_GAME_ID_TWIST_CUBE, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
        memcpy(bufAck, &tlvAck, sizeof(tlvAck));
        memcpy(bufAck + sizeof(tlvAck), &netId, sizeof(u16));
        broadcast(UNICAST, playerId, ACTION_CODE_GAME_DATA, bufAck, sizeof(tlvAck) + sizeof(u16));
    }
    else if (realAction == ACTION_CODE_START_GAME) {
        log_info("[RUBIK] Room %d: Game starting (triggered by player %d)", roomId, playerId);
        rs->status = 1;
        rs->eliminationTimer = 30.0f;
        rs->seed = (int)time(NULL);
        u32 net_seed = htonl((u32)rs->seed);

        u8 buf[64];
        memset(buf, 0, sizeof(buf));
        GameTLVHeader_St tlv_scr = { .gameId = MINI_GAME_ID_TWIST_CUBE, .action = ACTION_CODE_RUBIK_SCRAMBLE, .length = htons(sizeof(u32)) };
        memcpy(buf, &tlv_scr, sizeof(tlv_scr));
        memcpy(buf + sizeof(tlv_scr), &net_seed, sizeof(u32));
        broadcast(roomId, -1, ACTION_CODE_GAME_DATA, buf, sizeof(tlv_scr) + sizeof(u32));    }
    else if (realAction == ACTION_CODE_RUBIK_PROGRESS) {
        if (playerId < 0 || playerId >= MAX_CLIENTS) return;
        if (len >= sizeof(GameTLVHeader_St) + sizeof(float)) {
            memcpy(&rs->players[playerId].progress, realPayload, sizeof(float));
        }
    }
}

void twistCube_onTick(void* state) {
    RubikServerState* rs = (RubikServerState*)state;
    if (rs->status != 1) return;
    
    // Battle Royale Logic: Eliminate weakest every 30s
    // Simplified for now: just a placeholder logic
}

void twistCube_onPlayerLeave(void* state, s32 playerId) {
    if (playerId < 0 || playerId >= MAX_CLIENTS) return;
    RubikServerState* rs = (RubikServerState*)state;
    rs->players[playerId].active = false;
}

void twistCube_destroyInstance(void *state) {
    free(state);
}

GameServerInterface_St twistCube_serverInterface = {
    .gameName = "Twist Cube",
    .createInstance = twistCube_createInstance,
    .onAction = twistCube_onAction,
    .onTick = twistCube_onTick,
    .onPlayerLeave = twistCube_onPlayerLeave,
    .destroyInstance = twistCube_destroyInstance
};
