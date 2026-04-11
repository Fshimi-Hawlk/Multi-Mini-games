#include "networkInterface.h"
#include "APIs/generalAPI.h"
#include "APIs/rubikAPI.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

typedef struct {
    int id;
    float progress;
    bool active;
    bool eliminated;
} RubikPlayer;

typedef struct {
    RubikPlayer players[MAX_CLIENTS];
    int status; // 0: WAITING, 1: PLAYING
    float elimination_timer;
    int seed;
} RubikServerState;

void* rubik_create_instance(void) {
    RubikServerState* rs = calloc(1, sizeof(RubikServerState));
    if (rs) {
        rs->status = 0;
        rs->seed = (int)time(NULL);
        rs->elimination_timer = 0;
    }
    return rs;
}

void rubik_on_action(void *state, s32 room_id, s32 player_id, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA) return;
    if (len < sizeof(GameTLVHeader_St)) return;
    
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_CUBE) return;
    
    RubikServerState* rs = (RubikServerState*)state;
    u8 real_action = tlv->action;
    void* real_payload = (u8*)payload + sizeof(GameTLVHeader_St);
    (void)real_payload;

    if (real_action == ACTION_CODE_JOIN_GAME) {
        if (player_id < 0 || player_id >= MAX_CLIENTS) return;
        rs->players[player_id].active = true;
        rs->players[player_id].id = player_id;
        rs->players[player_id].progress = 0;
        rs->players[player_id].eliminated = false;
        
        int internal_id = player_id; // Simple mapping
        u16 net_id = htons((u16)internal_id);
        u8 buf_ack[64];
        memset(buf_ack, 0, sizeof(buf_ack));
        GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
        memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
        memcpy(buf_ack + sizeof(tlv_ack), &net_id, sizeof(u16));
        broadcast(UNICAST, player_id, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(u16));
    }
    else if (real_action == ACTION_CODE_START_GAME) {
        log_info("[RUBIK] Room %d: Game starting (triggered by player %d)", room_id, player_id);
        rs->status = 1;
        rs->elimination_timer = 30.0f;
        rs->seed = (int)time(NULL);
        u32 net_seed = htonl((u32)rs->seed);

        u8 buf[64];
        memset(buf, 0, sizeof(buf));
        GameTLVHeader_St tlv_scr = { .game_id = MINI_GAME_CUBE, .action = ACTION_CODE_RUBIK_SCRAMBLE, .length = htons(sizeof(u32)) };
        memcpy(buf, &tlv_scr, sizeof(tlv_scr));
        memcpy(buf + sizeof(tlv_scr), &net_seed, sizeof(u32));
        broadcast(room_id, -1, ACTION_CODE_GAME_DATA, buf, sizeof(tlv_scr) + sizeof(u32));    }
    else if (real_action == ACTION_CODE_RUBIK_PROGRESS) {
        if (player_id < 0 || player_id >= MAX_CLIENTS) return;
        if (len >= sizeof(GameTLVHeader_St) + sizeof(float)) {
            memcpy(&rs->players[player_id].progress, real_payload, sizeof(float));
        }
    }
}

void rubik_on_tick(void* state) {
    RubikServerState* rs = (RubikServerState*)state;
    if (rs->status != 1) return;
    
    // Battle Royale Logic: Eliminate weakest every 30s
    // Simplified for now: just a placeholder logic
}

void rubik_on_player_leave(void* state, s32 player_id) {
    if (player_id < 0 || player_id >= MAX_CLIENTS) return;
    RubikServerState* rs = (RubikServerState*)state;
    rs->players[player_id].active = false;
}

void rubik_destroy_instance(void *state) {
    free(state);
}

GameServerInterface_St rubikServerInterface = {
    .game_name = "rubik",
    .create_instance = rubik_create_instance,
    .on_action = rubik_on_action,
    .on_tick = rubik_on_tick,
    .on_player_leave = rubik_on_player_leave,
    .destroy_instance = rubik_destroy_instance
};
