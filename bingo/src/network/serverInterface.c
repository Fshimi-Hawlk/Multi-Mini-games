/**
    @file serverInterface.c
    @author Fshimi-Hawlk
    @date 2026-03-31
    @brief Server-side authoritative logic for the Bingo mini-game.
*/

#include "core/game.h"
#include "setups/game.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "networkInterface.h"

// 
// Action codes
// 

enum {
    ACTION_CODE_BINGO_CHOOSE_CARD = firstAvailableActionCode,
    ACTION_CODE_BINGO_DAUB_SQUARE,
    ACTION_CODE_BINGO_START_GAME,
    ACTION_CODE_BINGO_SYNC_STATE
};

#pragma pack(push, 1)
typedef struct {
    u8 cardIndex;   ///< 0..11
} ActionChooseCardPayload_St;

typedef struct {
    u8 row;
    u8 col;
} ActionDaubSquarePayload_St;
#pragma pack(pop)

/**
    @brief Minimal payload sent to clients for synchronization.
*/
#pragma pack(push, 1)
typedef struct {
    u32              remainingBalls;
    CallState_St     currentCall;
    GameScene_Et     scene;
    u32              numPlayers;
    char             resultMessage[64];
} BingoSyncPayload_St;
#pragma pack(pop)

/**
    @brief Game status for Bingo.
*/
typedef enum {
    BINGO_STATUS_WAITING_CARD_CHOICE,
    BINGO_STATUS_LAUNCHING,
    BINGO_STATUS_PLAYING,
    BINGO_STATUS_ENDED
} BingoStatus_Et;

/**
    @brief Server-side wrapper that holds all per-instance state.
*/
typedef struct {
    BingoGame_St        game;                       ///< Shared state (MUST be first)
    u32                 numPlayers;                 ///< Current number of players
    PlayerCard_St       playerCards[MAX_PLAYER];    ///< Per-player cards
    s32                 playerNetworkIds[MAX_PLAYER]; ///< slot -> network player_id mapping
    BingoStatus_Et      status;                     ///< Current phase
    BroadcastMessage_Ft serverBroadcastCallback;    ///< Broadcast function
} BingoServerState_St;

// 
// Helper to find slot from player_id
// 
static s32 bingo_getSlot(BingoServerState_St* srv, s32 player_id) {
    for (u32 i = 0; i < srv->numPlayers; i++) {
        if (srv->playerNetworkIds[i] == player_id) return (s32)i;
    }
    return -1;
}

// 
// Instance Management
// 

void* bingo_createInstance(void) {
    BingoServerState_St* srv = calloc(1, sizeof(BingoServerState_St));
    if (srv == NULL) return NULL;

    BingoGame_St* game = &srv->game;
    game->base.running = true;
    srv->status        = BINGO_STATUS_WAITING_CARD_CHOICE;
    srv->numPlayers    = 0;

    // Initialize network IDs
    for (int i = 0; i < MAX_PLAYER; i++) srv->playerNetworkIds[i] = -1;

    // Generate 12 preview cards
    uint available[100];
    for (uint i = 0; i < 100; ++i) available[i] = i;
    for (uint i = 0; i < 12; ++i) {
        bingo_generateCard(game->layout.choiceCards[i].values, available, 100);
    }

    // Ball system
    game->balls.remainingCount = 500;
    uint b = 0;
    for (uint n = 0; n < 100; ++n) {
        for (uint col = 1; col <= 5; ++col) {
            game->balls.encodedBalls[b++] = 100 * col + n;
        }
    }
    shuffleArray(uint, game->balls.encodedBalls, 500, prng_rand);

    game->balls.choiceDelay = 3.5f;
    game->balls.showDelay   = 1.5f;
    game->balls.graceDelay  = 1.0f;
    game->currentCall.timer = game->balls.showDelay;
    game->progress.scene = GAME_SCENE_CARD_CHOICE;

    return srv;
}

static void bingo_serverBroadcastSync(BingoServerState_St* srv, s32 room_id, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;

    BingoGame_St* game = &srv->game;
    BingoSyncPayload_St payload = {
        .remainingBalls  = game->balls.remainingCount,
        .currentCall     = game->currentCall,
        .scene           = game->progress.scene,
        .numPlayers      = srv->numPlayers
    };
    snprintf(payload.resultMessage, 63, "%s", game->progress.resultMessage);

    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = ACTION_CODE_BINGO_SYNC_STATE,
        .length  = htons(sizeof(BingoSyncPayload_St))
    };

    u8 buf[1024];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &tlv, sizeof(tlv));
    memcpy(buf + sizeof(tlv), &payload, sizeof(payload));

    broadcast(room_id, -1, ACTION_CODE_GAME_DATA, buf, sizeof(tlv) + sizeof(payload));
}

void bingo_onAction(void* state, s32 room_id, s32 player_id, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA || len < sizeof(GameTLVHeader_St)) return;

    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_BINGO) return;

    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);
    u16 payloadLen = len - sizeof(GameTLVHeader_St);

    BingoServerState_St* srv = (BingoServerState_St*)state;
    BingoGame_St* game = &srv->game;
    srv->serverBroadcastCallback = broadcast;

    switch (realAction) {
        case ACTION_CODE_JOIN_GAME: {
            if (srv->numPlayers >= MAX_PLAYER) break;
            if (bingo_getSlot(srv, player_id) != -1) break; // Already in

            u32 slot = srv->numPlayers++;
            srv->playerNetworkIds[slot] = player_id;
            u32 randomCard = rand() % 12;

            memcpy(&srv->playerCards[slot], &game->layout.choiceCards[randomCard].values, sizeof(Card_t));
            srv->playerCards[slot].daubs[2][2] = true; // Free space
            log_info("Player %d joined Bingo (slot %u)", player_id, slot);

            u8 buf_ack[64];
            memset(buf_ack, 0, sizeof(buf_ack));
            GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_BINGO, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
            u16 net_id = htons((u16)slot);
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &net_id, sizeof(u16));
            broadcast(UNICAST, player_id, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(u16));
        } break;

        case ACTION_CODE_BINGO_CHOOSE_CARD: {
            if (payloadLen < sizeof(ActionChooseCardPayload_St)) break;
            ActionChooseCardPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            s32 slot = bingo_getSlot(srv, player_id);
            if (slot != -1 && p.cardIndex < 12) {
                memcpy(&srv->playerCards[slot], &game->layout.choiceCards[p.cardIndex].values, sizeof(Card_t));
                srv->playerCards[slot].daubs[2][2] = true; // Free space
                log_info("Player %d chose card %u (slot %d)", player_id, p.cardIndex, slot);
            }
        } break;

        case ACTION_CODE_BINGO_DAUB_SQUARE: {
            if (payloadLen < sizeof(ActionDaubSquarePayload_St)) break;
            if (srv->status != BINGO_STATUS_PLAYING) break;

            ActionDaubSquarePayload_St p;
            memcpy(&p, realPayload, sizeof(p));
            if (p.row >= 5 || p.col >= 5) break; // FIX 1: OOB protection
            s32 slot = bingo_getSlot(srv, player_id);
            if (slot != -1 && bingo_isValidDaub(&game->currentCall, &srv->playerCards[slot], p.row, p.col)) {
                srv->playerCards[slot].daubs[p.row][p.col] = true;
            }
        } break;

        case ACTION_CODE_START_GAME:
        case ACTION_CODE_BINGO_START_GAME: {
            if (srv->status != BINGO_STATUS_WAITING_CARD_CHOICE) break;
            game->room_id = room_id;
            srv->status = BINGO_STATUS_LAUNCHING;
            game->progress.scene = GAME_SCENE_LAUNCHING; // Tell clients to transition from card choice
            game->currentCall.timer = 6.5f;
            log_info("Bingo game launching...");
        } break;
    }

    bingo_serverBroadcastSync(srv, room_id, broadcast);
}

void bingo_onTick(void* state) {
    BingoServerState_St* srv = (BingoServerState_St*)state;
    if (!srv->serverBroadcastCallback) return;
    if (srv->status != BINGO_STATUS_PLAYING && srv->status != BINGO_STATUS_LAUNCHING) return;

    BingoGame_St* game = &srv->game;
    f32 dt = 0.016f; 

    if (srv->status == BINGO_STATUS_LAUNCHING) {
        game->currentCall.timer -= dt;
        if (game->currentCall.timer <= 0.0f) {
            game->currentCall.timer = 0.0f;
            srv->status = BINGO_STATUS_PLAYING;
            game->progress.scene = GAME_SCENE_PLAYING;
        }
        bingo_serverBroadcastSync(srv, game->room_id, srv->serverBroadcastCallback);
        return;
    }

    game->currentCall.timer += dt;
    if (game->currentCall.timer >= game->balls.choiceDelay) {
        game->currentCall.timer = 0.0f;
        if (game->balls.remainingCount > 0) {
            game->currentCall.encodedValue = game->balls.encodedBalls[--game->balls.remainingCount];
            game->currentCall.column = (game->currentCall.encodedValue / 100) - 1;
            game->currentCall.number = game->currentCall.encodedValue - (game->currentCall.column + 1) * 100;
            snprintf(game->currentCall.displayedText, 15, "%s %u", LETTERS[game->currentCall.column], game->currentCall.number);
        }
    }

    // Win check
    for (u32 i = 0; i < srv->numPlayers; ++i) {
        if (bingo_hasBingo(&srv->playerCards[i])) {
            game->progress.scene = GAME_SCENE_END;
            snprintf(game->progress.resultMessage, 63, "BINGO! Player %d wins!", srv->playerNetworkIds[i]);
            srv->status = BINGO_STATUS_ENDED;
            break;
        }
    }

    if (game->balls.remainingCount == 0 && srv->status != BINGO_STATUS_ENDED) {
        game->progress.scene = GAME_SCENE_END;
        snprintf(game->progress.resultMessage, 63, "No more balls - Game Over");
        srv->status = BINGO_STATUS_ENDED;
    }

    bingo_serverBroadcastSync(srv, game->room_id, srv->serverBroadcastCallback);
}

void bingo_onPlayerLeave(void* state, s32 player_id) {
    BingoServerState_St* srv = (BingoServerState_St*)state;
    s32 slot = bingo_getSlot(srv, player_id);
    if (slot == -1) return;

    for (u32 i = (u32)slot; i < srv->numPlayers - 1; ++i) {
        srv->playerCards[i] = srv->playerCards[i + 1];
        srv->playerNetworkIds[i] = srv->playerNetworkIds[i + 1];
    }
    srv->numPlayers--;
    log_info("Player %d left (slot %d vacated)", player_id, slot);
}

void bingo_destroyInstance(void* state) {
    if (state) free(state);
}

GameServerInterface_St bingoServerInterface = {
    .game_name        = "Bingo",
    .create_instance  = bingo_createInstance,
    .on_action        = bingo_onAction,
    .on_tick          = bingo_onTick,
    .on_player_leave  = bingo_onPlayerLeave,
    .destroy_instance = bingo_destroyInstance
};
