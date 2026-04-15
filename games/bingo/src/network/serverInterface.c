/**
    @file serverInterface.c
    @author Fshimi-Hawlk
    @date 2026-03-11
    @date 2026-04-14
    @brief Server-side authoritative logic for the Bingo mini-game.
*/
#include "core/game.h"

#include "setups/game.h"

#include "utils/globals.h"

#include "sharedUtils/random.h"

/**
    @brief Action codes for network communication.
           Must stay in sync with the client implementation.
*/
enum {
    ACTION_CODE_BINGO_CHOOSE_CARD = firstAvailableActionCode,
    ACTION_CODE_BINGO_DAUB_SQUARE,
    ACTION_CODE_BINGO_START_GAME,
    ACTION_CODE_BINGO_SYNC_STATE
};

#pragma pack(push, 1)
/**
    @brief Payload sent when client selects one of the 12 preview cards.
*/
typedef struct {
    u8 cardIndex;   ///< 0..11 – index into layout.choiceCards
} ActionChooseCardPayload_St;

/**
    @brief Payload for a daub attempt on the player’s 5×5 card.
*/
typedef struct {
    u8 row;         ///< Row index (0-4)
    u8 col;         ///< Column index (0-4)
} ActionDaubSquarePayload_St;
#pragma pack(pop)

/**
    @brief Minimal payload sent to clients for synchronization.
*/
#pragma pack(push, 1)
typedef struct {
    u32              remainingBalls;        ///< Number of balls remaining in the hopper
    CallState_St     currentCall;           ///< Current called number and column
    GameScene_Et     scene;                 ///< Current game scene
    u32              numPlayers;            ///< Number of connected players
    u32              seed;                  ///< Random seed for card generation
    char             resultMessage[64];     ///< Message to display at the end of the game
    PlayerCard_St    playerCards[MAX_PLAYER]; ///< State of all players' cards
    s32              playerNetworkIds[MAX_PLAYER]; ///< Network IDs of all players
} BingoSyncPayload_St;
#pragma pack(pop)

/**
    @brief Internal server-side game status for Bingo.
*/
typedef enum {
    BINGO_STATUS_WAITING_CARD_CHOICE,        ///< Waiting for players to choose their cards
    BINGO_STATUS_LAUNCHING,                 ///< Game is starting (countdown)
    BINGO_STATUS_PLAYING,                   ///< Game is currently in progress
    BINGO_STATUS_ENDED                      ///< Game has ended
} BingoStatus_Et;

/**
    @brief Server-side wrapper that holds all per-instance state.
*/
typedef struct {
    BingoGame_St        game;                       ///< Shared state (MUST be first)
    u32                 numPlayers;                 ///< Current number of players
    PlayerCard_St       playerCards[MAX_PLAYER];    ///< Per-player cards
    s32                 playerNetworkIds[MAX_PLAYER]; ///< slot -> network playerId mapping
    BingoStatus_Et      status;                     ///< Current phase
    BroadcastMessage_Ft serverBroadcastCallback;    ///< Broadcast function
    u32                 seed;                       ///< Common seed for card generation
} BingoServerState_St;

/**
    @brief Helper to find the slot index corresponding to a network player_id.
    @param[in]     srv          Pointer to the server state.
    @param[in]     player_id    The network player ID to search for.
    @return                     The slot index, or -1 if not found.
*/
// ────────────────────────────────────────────────
// Helper to find slot from playerId
// ────────────────────────────────────────────────

static s32 bingo_getSlot(BingoServerState_St* srv, s32 playerId) {
    for (u32 i = 0; i < srv->numPlayers; i++) {
        if (srv->playerNetworkIds[i] == playerId) return (s32)i;
    }
    return -1;
}

/**
    @brief Creates a new server-side instance of the Bingo game.
    @param[in]     void
    @return                     Pointer to the new instance state.
*/
void* bingo_createInstance(void) {
    BingoServerState_St* srv = calloc(1, sizeof(BingoServerState_St));
    if (srv == NULL) return NULL;

    srv->seed = (u32)time(NULL);
    srand(srv->seed);

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

    shuffleArrayT(uint, game->balls.encodedBalls, 500, rand);

    game->balls.choiceDelay = 3.5f;
    game->balls.showDelay   = 1.5f;
    game->balls.graceDelay  = 1.0f;
    game->currentCall.timer = game->balls.showDelay;
    game->progress.scene = GAME_SCENE_CARD_CHOICE;

    return srv;
}

/**
    @brief Broadcasts the current game state to all connected clients.
    @param[in]     srv          Pointer to the server state.
    @param[in]     room_id      The room ID to broadcast to.
    @param[in]     broadcast    The broadcast function pointer.
    @return                     void
*/
static void bingo_serverBroadcastSync(BingoServerState_St* srv, s32 roomId, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;

    BingoGame_St* game = &srv->game;
    BingoSyncPayload_St payload = {
        .remainingBalls  = game->balls.remainingCount,
        .currentCall     = game->currentCall,
        .scene           = game->progress.scene,
        .numPlayers      = srv->numPlayers,
        .seed            = srv->seed
    };
    snprintf(payload.resultMessage, sizeof(payload.resultMessage), "%s", game->progress.resultMessage);
    
    for (int i = 0; i < MAX_PLAYER; i++) {
        payload.playerCards[i] = srv->playerCards[i];
        payload.playerNetworkIds[i] = srv->playerNetworkIds[i];
    }

    GameTLVHeader_St tlv = {
        .gameId = MINI_GAME_ID_BINGO,
        .action  = ACTION_CODE_BINGO_SYNC_STATE,
        .length  = htons(sizeof(BingoSyncPayload_St))
    };

    u8 buf[2048]; // Increased buffer size for larger payload
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &tlv, sizeof(tlv));
    memcpy(buf + sizeof(tlv), &payload, sizeof(payload));

    broadcast(roomId, -1, ACTION_CODE_GAME_DATA, buf, (u16)(sizeof(tlv) + sizeof(payload)));
}

/**
    @brief Callback for when a client performs an action.
    @param[in,out] state        Pointer to the instance state.
    @param[in]     room_id      The room ID.
    @param[in]     player_id    The network player ID.
    @param[in]     action       The action code.
    @param[in]     payload      The payload data.
    @param[in]     len          The length of the payload data.
    @param[in]     broadcast    The broadcast function pointer.
    @return                     void
*/
void bingo_onAction(void* state, s32 roomId, s32 playerId, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA || len < sizeof(GameTLVHeader_St)) return;

    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->gameId != MINI_GAME_ID_BINGO) return;

    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);
    u16 payloadLen = len - sizeof(GameTLVHeader_St);

    BingoServerState_St* srv = (BingoServerState_St*)state;
    BingoGame_St* game = &srv->game;
    srv->serverBroadcastCallback = broadcast;

    switch (realAction) {
        case ACTION_CODE_JOIN_GAME: {
            if (srv->numPlayers >= MAX_PLAYER) {
                log_warn("Bingo room full, rejecting player %d", playerId);
                break;
            }
            if (bingo_getSlot(srv, playerId) != -1) break; // Already in

            u32 slot = srv->numPlayers++;
            srv->playerNetworkIds[slot] = playerId;
            u32 randomCard = rand() % 12;

            memcpy(&srv->playerCards[slot], &game->layout.choiceCards[randomCard].values, sizeof(Card_t));
            srv->playerCards[slot].daubs[2][2] = true; // Free space
            log_info("Player %d joined Bingo (slot %u)", playerId, slot);

            u8 buf_ack[64];
            memset(buf_ack, 0, sizeof(buf_ack));
            GameTLVHeader_St tlv_ack = { .gameId = MINI_GAME_ID_BINGO, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
            u16 netId = htons((u16)slot);
            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &netId, sizeof(u16));
            broadcast(UNICAST, playerId, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(u16));
        } break;

        case ACTION_CODE_BINGO_CHOOSE_CARD: {
            if (payloadLen < sizeof(ActionChooseCardPayload_St)) break;
            ActionChooseCardPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            s32 slot = bingo_getSlot(srv, playerId);
            if (slot != -1 && p.cardIndex < 12) {
                memcpy(&srv->playerCards[slot], &game->layout.choiceCards[p.cardIndex].values, sizeof(Card_t));
                srv->playerCards[slot].daubs[2][2] = true; // Free space
                log_info("Player %d chose card %u (slot %d)", playerId, p.cardIndex, slot);
            }
        } break;

        case ACTION_CODE_BINGO_DAUB_SQUARE: {
            if (payloadLen < sizeof(ActionDaubSquarePayload_St)) break;
            if (srv->status != BINGO_STATUS_PLAYING) break;

            ActionDaubSquarePayload_St p;
            memcpy(&p, realPayload, sizeof(p));
            if (p.row >= 5 || p.col >= 5) break; 
            s32 slot = bingo_getSlot(srv, playerId);
            if (slot == -1) break;

            if (bingo_isValidDaub(&game->currentCall, &srv->playerCards[slot], p.row, p.col)) {
                srv->playerCards[slot].daubs[p.row][p.col] = true;
                srv->playerCards[slot].misclicks[p.row][p.col] = 0;
                
                // Check for victory
                if (bingo_hasBingo(&srv->playerCards[slot])) {
                    srv->status = BINGO_STATUS_ENDED;
                    game->progress.scene = GAME_SCENE_END;
                    snprintf(game->progress.resultMessage, 63, "BINGO! Player %d wins!", playerId);
                    log_info("Bingo: Player %d wins in room %d", playerId, roomId);
                }
            } else {
                srv->playerCards[slot].misclicks[p.row][p.col]++;
            }
        } break;

        case ACTION_CODE_START_GAME:
        case ACTION_CODE_BINGO_START_GAME: {
            if (srv->status != BINGO_STATUS_WAITING_CARD_CHOICE) break;
            game->roomId = roomId;
            srv->status = BINGO_STATUS_LAUNCHING;
            game->progress.scene = GAME_SCENE_LAUNCHING; // Tell clients to transition from card choice
            game->currentCall.timer = 6.5f;
            log_info("Bingo game launching...");
        } break;
    }

    bingo_serverBroadcastSync(srv, roomId, broadcast);
}

/**
    @brief Updates the server-side game state on each tick.
    @param[in,out] state        Pointer to the instance state.
    @return                     void
*/
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
        bingo_serverBroadcastSync(srv, game->roomId, srv->serverBroadcastCallback);
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

    bingo_serverBroadcastSync(srv, game->roomId, srv->serverBroadcastCallback);
}

/**
    @brief Callback for when a player leaves the room.
    @param[in,out] state        Pointer to the instance state.
    @param[in]     player_id    The network player ID who left.
    @return                     void
*/
void bingo_onPlayerLeave(void* state, s32 playerId) {
    BingoServerState_St* srv = (BingoServerState_St*)state;
    s32 slot = bingo_getSlot(srv, playerId);
    if (slot == -1) return;

    for (u32 i = (u32)slot; i < srv->numPlayers - 1; ++i) {
        srv->playerCards[i] = srv->playerCards[i + 1];
        srv->playerNetworkIds[i] = srv->playerNetworkIds[i + 1];
    }
    srv->numPlayers--;
    log_info("Player %d left (slot %d vacated)", playerId, slot);
}

/**
    @brief Destroys a server-side instance of the Bingo game.
    @param[in]     state        Pointer to the instance state.
    @return                     void
*/
void bingo_destroyInstance(void* state) {
    if (state) free(state);
}

/**
    @brief Server interface definition for the Bingo mini-game.
*/
GameServerInterface_St bingoServerInterface = {
    .gameName        = "Bingo",
    .createInstance  = bingo_createInstance,
    .onAction        = bingo_onAction,
    .onTick          = bingo_onTick,
    .onPlayerLeave  = bingo_onPlayerLeave,
    .destroyInstance = bingo_destroyInstance
};
