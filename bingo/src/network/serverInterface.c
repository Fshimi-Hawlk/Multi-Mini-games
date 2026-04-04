/**
    @file serverInterface.c
    @author Fshimi-Hawlk
    @date 2026-03-31
    @date 2026-03-31
    @brief Server-side authoritative logic for the Bingo mini-game.
  
    Contributors:
        - Fshimi-Hawlk:
            - Authoritative ball drawing and daub validation
            - Per-player card assignment from shared 12-card pool
            - Minimal network sync payload (no full struct dump)
*/

#include "core/game.h"

#include "setups/game.h"

#include "utils/globals.h"

#include "utils/random.h"
#include "utils/debug.h"

#include "networkInterface.h"

// ─────────────────────────────────────────────────────────────────────────────
// Action codes
// ─────────────────────────────────────────────────────────────────────────────

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
           Only fields that actually change during play are included.
*/
#pragma pack(push, 1)
typedef struct {
    u32              remainingBalls;
    CallState_St     currentCall;
    GameScene_Et     scene;
    const char*      resultMessage;   // pointer is safe because it's a literal or static
    // Per-player daubs would be sent separately or compressed later
} BingoSyncPayload_St;
#pragma pack(pop)

/**
    @brief Game status for Bingo – used on both client and server.
*/
typedef enum {
    BINGO_STATUS_WAITING_CARD_CHOICE,   ///< Players are selecting from the 12 preview cards
    BINGO_STATUS_LAUNCHING,             ///< Countdown before main game starts
    BINGO_STATUS_PLAYING,               ///< Active gameplay with ball draws and daubing
    BINGO_STATUS_ENDED                  ///< Game finished (win or no more balls)
} BingoStatus_Et;

// ─────────────────────────────────────────────────────────────────────────────
// Server-only state
// ─────────────────────────────────────────────────────────────────────────────

static u32                 numPlayers;
static PlayerCard_St       playerCards[MAX_PLAYER];
static BingoStatus_Et      status;
static BroadcastMessage_Ft serverBroadcastCallback;

// ─────────────────────────────────────────────────────────────────────────────

void* bingo_createInstance(void) {
    BingoGame_St* game = calloc(1, sizeof(BingoGame_St));
    if (game == NULL) {
        log_error("Failed to allocate BingoGame_St for server");
        return NULL;
    }

    game->base.running = true;
    status             = BINGO_STATUS_WAITING_CARD_CHOICE;
    numPlayers         = 0;

    // Free space
    game->player.daubs[2][2]   = true;
    game->player.numbers[2][2] = UINT32_MAX;

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

    shuffleArrayT(uint, game->balls.encodedBalls, 500, rand);

    game->balls.choiceDelay = 3.5f;
    game->balls.showDelay   = 1.5f;
    game->balls.graceDelay  = 1.0f;

    game->currentCall.timer = game->balls.showDelay;
    game->currentCall.displayedText[0] = '\0';

    game->progress.scene = GAME_SCENE_CARD_CHOICE;

    log_info("Bingo server instance created");
    return game;
}

static void bingo_serverBroadcastSync(BingoGame_St* game, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;

    BingoSyncPayload_St payload = {
        .remainingBalls  = game->balls.remainingCount,
        .currentCall     = game->currentCall,
        .scene           = game->progress.scene,
        .resultMessage   = game->progress.resultMessage
    };

    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = ACTION_CODE_BINGO_SYNC_STATE,
        .length  = sizeof(BingoSyncPayload_St)
    };

    u8 buf[512];
    memcpy(buf, &tlv, sizeof(tlv));
    memcpy(buf + sizeof(tlv), &payload, sizeof(payload));

    broadcast(0, -1, 5, buf, sizeof(tlv) + sizeof(payload));
}

void bingo_onAction(void* state, int playerID, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA) return;

    if (len < sizeof(GameTLVHeader_St)) return;
    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_BINGO) return; 

    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);

    BingoGame_St* game = (BingoGame_St*)state;
    serverBroadcastCallback = broadcast;

    switch (realAction) {
        case ACTION_CODE_JOIN_GAME: {
            if (numPlayers >= MAX_PLAYER) break;

            u32 slot = numPlayers++;
            u32 randomCard = rand() % 12;
            
            memcpy(&playerCards[slot], &game->layout.choiceCards[randomCard].values, sizeof(Card_t));
            log_info("Player %d joined Bingo (slot %u)", playerID, slot);

            u8 buf_ack[1024];
            GameTLVHeader_St tlv_ack = { .game_id = MINI_GAME_KFF, .action = ACTION_CODE_JOIN_ACK, .length = sizeof(int) };
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &game->clientID, sizeof(int));
            broadcast(UNICAST, playerID, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(int));
        } break;

        case ACTION_CODE_BINGO_CHOOSE_CARD: {
            if (game->clientID == -1) break;
            if (len < sizeof(ActionChooseCardPayload_St)) break;

            ActionChooseCardPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            if (p.cardIndex < 12 && (u32)playerID < numPlayers) {
                u32 slot = (u32)playerID;
                memcpy(&playerCards[slot], &game->layout.choiceCards[p.cardIndex].values, sizeof(Card_t));
                log_info("Player %d chose card %u", playerID, p.cardIndex);
            }
        } break;

        case ACTION_CODE_BINGO_DAUB_SQUARE: {
            if (len < sizeof(ActionDaubSquarePayload_St)) break;
            if (status != BINGO_STATUS_PLAYING) break;

            ActionDaubSquarePayload_St p;
            memcpy(&p, realPayload, sizeof(p));
            u32 slot = (u32)playerID;
            if (slot < numPlayers &&
                bingo_isValidDaub(&game->currentCall, &playerCards[slot], p.row, p.col)) {
                playerCards[slot].daubs[p.row][p.col] = true;
            }
        } break;

        case ACTION_CODE_BINGO_START_GAME: {
            if (status != BINGO_STATUS_WAITING_CARD_CHOICE) break;

            status = BINGO_STATUS_LAUNCHING;
            game->currentCall.timer = 6.5f;
            log_info("Bingo game started by host");
        } break;

        default: log_warn("Action code (%d) not Implemented", action);
    }

    bingo_serverBroadcastSync(game, broadcast);
}

void bingo_onTick(void* state) {
    BingoGame_St* game = (BingoGame_St*)state;
    if (status != BINGO_STATUS_PLAYING || !serverBroadcastCallback) return;

    f32 dt = 0.016f; // ~60 Hz
    game->currentCall.timer += dt;

    if (game->currentCall.timer >= game->balls.choiceDelay) {
        game->currentCall.timer = 0.0f;

        if (game->balls.remainingCount > 0) {
            game->currentCall.encodedValue = game->balls.encodedBalls[--game->balls.remainingCount];
            game->currentCall.column = (game->currentCall.encodedValue / 100) - 1;
            game->currentCall.number = game->currentCall.encodedValue - (game->currentCall.column + 1) * 100;

            sprintf(
                game->currentCall.displayedText, "%s %u",
                LETTERS[game->currentCall.column], game->currentCall.number
            );
        }
    }

    // Win check
    for (u32 i = 0; i < numPlayers; ++i) {
        if (bingo_hasBingo(&playerCards[i])) {
            game->progress.scene         = GAME_SCENE_END;
            game->progress.resultMessage = "BINGO! Player wins!";
            status = BINGO_STATUS_ENDED;
            break;
        }
    }

    if (game->balls.remainingCount == 0) {
        game->progress.scene         = GAME_SCENE_END;
        game->progress.resultMessage = "No more balls - Game Over";
        status = BINGO_STATUS_ENDED;
    }

    bingo_serverBroadcastSync(game, serverBroadcastCallback);
}

void bingo_onPlayerLeave(void* state, int playerID) {
    UNUSED(state);
    
    if (playerID < 0 || (u32)playerID >= numPlayers) return;

    // Simple shift for demo
    for (u32 i = (u32)playerID; i < numPlayers - 1; ++i) {
        playerCards[i] = playerCards[i + 1];
    }
    numPlayers--;
    log_info("Player %d left Bingo room", playerID);
}

void bingo_destroyInstance(void* state) {
    BingoGame_St* game = (BingoGame_St*) state;

    if (game) {
        free(game);
    }
}

GameServerInterface_St bingoServerInterface = {
    .game_name        = "Bingo",
    .create_instance  = bingo_createInstance,
    .on_action        = bingo_onAction,
    .on_tick          = bingo_onTick,
    .on_player_leave  = bingo_onPlayerLeave,
    .destroy_instance = bingo_destroyInstance
};