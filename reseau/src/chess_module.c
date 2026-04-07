#include "networkInterface.h"
#include "APIs/generalAPI.h"
#include "APIs/chessAPI.h"
#include <stdio.h>
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
    int num_players;
    int turn; // 0 or 1
    // Board state could be here too if we wanted authoritative server
} ChessServerState;

void* chess_create_instance(void) {
    ChessServerState* cs = calloc(1, sizeof(ChessServerState));
    if (cs) {
        cs->players[0] = -1;
        cs->players[1] = -1;
        cs->num_players = 0;
        cs->turn = 0;
    }
    return cs;
}

void chess_on_action(void *state, s32 room_id, s32 player_id, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA) return;
    if (len < sizeof(GameTLVHeader_St)) return;
    
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_CHESS) return;
    
    ChessServerState* cs = (ChessServerState*)state;
    u8 real_action = tlv->action;
    void* real_payload = (u8*)payload + sizeof(GameTLVHeader_St);
    (void)real_payload;

    if (real_action == ACTION_CODE_JOIN_GAME) {
        int internal_id = -1;
        // Assign white then black
        if (cs->players[0] == -1) {
            cs->players[0] = player_id;
            internal_id = 0;
            cs->num_players++;
        } else if (cs->players[1] == -1 && cs->players[0] != player_id) {
            cs->players[1] = player_id;
            internal_id = 1;
            cs->num_players++;
        }
        
        if (internal_id != -1) {
            u8 buf_ack[64];
            memset(buf_ack, 0, sizeof(buf_ack));
            GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_CHESS, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(int)) };
            int net_internal_id = htonl(internal_id);
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &net_internal_id, sizeof(int));
            broadcast(UNICAST, player_id, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(int));
        }
    }
    else if (real_action == ACTION_CODE_START_GAME) {
        // Broadcast game start to everyone else
        broadcast(room_id, player_id, ACTION_CODE_GAME_DATA, payload, len);
        // Also send back to host so they start the game too
        broadcast(UNICAST, player_id, ACTION_CODE_GAME_DATA, payload, len);
    }
    else if (real_action == ACTION_CODE_CHESS_MOVE) {
        // Broadcast move to all (including sender for simplicity or excluding)
        // In our case, we broadcast to everyone so the other player sees it.
        broadcast(room_id, player_id, ACTION_CODE_GAME_DATA, payload, len);
        cs->turn = !cs->turn;
    }
}

void chess_on_tick(void* state) {
    (void)state;
}

void chess_on_player_leave(void* state, s32 player_id) {
    ChessServerState* cs = (ChessServerState*)state;
    if (cs->players[0] == player_id) cs->players[0] = -1;
    if (cs->players[1] == player_id) cs->players[1] = -1;
}

void chess_destroy_instance(void *state) {
    free(state);
}

GameServerInterface_St chessServerInterface = {
    .game_name = "chess",
    .create_instance = chess_create_instance,
    .on_action = chess_on_action,
    .on_tick = chess_on_tick,
    .on_player_leave = chess_on_player_leave,
    .destroy_instance = chess_destroy_instance
};
