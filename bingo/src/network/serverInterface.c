/**
    @file network/serverInterface.c
    @author Fshimi-Hawlk
    @date 2026-03-31
    @date 2026-04-05
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
#include "APIs/generalAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Action codes
// ─────────────────────────────────────────────────────────────────────────────

enum {
    ACTION_CODE_BINGO_PLAYER_READY = firstAvailableActionCode,
    ACTION_CODE_BINGO_SET_MAX_PLAYERS,
    ACTION_CODE_BINGO_SEND_CHOICE_CARDS,
    ACTION_CODE_BINGO_CHOOSE_CARD,
    ACTION_CODE_BINGO_LOCK_CARD,
    ACTION_CODE_BINGO_SYNC_STATE,
    ACTION_CODE_BINGO_DAUB_SQUARE,
};

/**
    @brief Game gameStatus for Bingo - used on both client and server.
*/
typedef enum {
    BINGO_STATUS_WAITING_FOR_PLAYER,    ///< Waiting for at least 2 players to join (host + another)
    BINGO_STATUS_WAITING_CARD_CHOICE,   ///< Players are selecting from the 12 preview cards
    BINGO_STATUS_LAUNCHING,             ///< Countdown before main game starts
    BINGO_STATUS_PLAYING,               ///< Active gameplay with ball draws and daubing
    BINGO_STATUS_ENDED                  ///< Game finished (win or no more balls)
} BingoStatus_Et;

#pragma pack(push, 1)
typedef struct {
    u8 cardIndex;   ///< 0..11
} ActionChooseCardPayload_St;

typedef struct {
    u8 cardIndex;   ///< 0..11 – the card the player is locking
} ActionLockCardPayload_St;

typedef struct {
    u8 row;
    u8 col;
} ActionDaubSquarePayload_St;

typedef struct {
    u8 maxPlayers;  ///< 2..4
} ActionSetMaxPlayersPayload_St;

/**
    @brief One-time payload sent exactly once when entering the card-choice phase.
           Contains the 12 preview cards so clients can render them locally.
*/
typedef struct {
    Card_t choiceCards[12];
} BingoChoiceCardsPayload_St;

/**
    @brief Minimal payload sent to clients for synchronization.
           Only fields that actually change during play are included.
*/
typedef struct {
    f32             timer;                  ///< currentCall.timer
    char            displayedText[8];      ///< currentCall.displayedText
    GameScene_Et    scene;
    const char     *resultMessage;
    u8              numPlayers;
    u8              maxPlayers;
    u8              playerColors[MAX_PLAYER];   ///< 0-3 for visual distinction
    u8              cardOwners[12];             ///< MAX_PLAYER = free, otherwise player slot
} BingoSyncPayload_St;
#pragma pack(pop)

// ─────────────────────────────────────────────────────────────────────────────
// Server-only state
// ─────────────────────────────────────────────────────────────────────────────

static u32                 numPlayers;
static PlayerCard_St       players[MAX_PLAYER];
static bool                playersReady[MAX_PLAYER];
static u8                  playerColors[MAX_PLAYER];      ///< 0-3 for visual distinction
static u8                  playerChosenCard[MAX_PLAYER];  ///< MAX_PLAYER = not locked yet
static u8                  cardOwners[12];                ///< MAX_PLAYER = free
static u8                  maxPlayers;
static BingoStatus_Et      gameStatus;
static BroadcastMessage_Ft serverBroadcastCallback;
static GameInstance_St*    currentInstance;

// ─────────────────────────────────────────────────────────────────────────────

void* bingo_createInstance(void) {
    BingoGame_St* game = calloc(1, sizeof(BingoGame_St));
    if (game == NULL) {
        log_error("Failed to allocate BingoGame_St for server");
        return NULL;
    }

    game->base.running = true;
    gameStatus         = BINGO_STATUS_WAITING_FOR_PLAYER;
    numPlayers         = 0;
    maxPlayers         = MAX_PLAYER;

    memset(playersReady, 0, sizeof(playersReady));
    memset(playerColors, 0, sizeof(playerColors));
    memset(playerChosenCard, MAX_PLAYER, sizeof(playerChosenCard));   // not locked
    memset(cardOwners,      MAX_PLAYER, sizeof(cardOwners));          // free

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

    game->progress.scene = GAME_SCENE_WAITING_ROOM;

    log_info("Bingo server instance created");
    return game;
}

static void bingo_serverBroadcastSync(BingoGame_St* game, GameInstance_St* instance, BroadcastMessage_Ft broadcast) {
    if (broadcast == NULL || instance == NULL || game == NULL) return;

    BingoSyncPayload_St payload = {
        .timer           = game->currentCall.timer,
        .scene           = game->progress.scene,
        .resultMessage   = game->progress.resultMessage,
        .numPlayers      = (u8)numPlayers,
        .maxPlayers      = maxPlayers,
    };

    strncpy(payload.displayedText, game->currentCall.displayedText, sizeof(payload.displayedText)-1);
    payload.displayedText[sizeof(payload.displayedText)-1] = '\0';

    for (u8 i = 0; i < MAX_PLAYER; ++i) {
        payload.playerColors[i] = (i < numPlayers) ? playerColors[i] : MAX_PLAYER;
        payload.cardOwners[i]   = (i < 12) ? cardOwners[i] : MAX_PLAYER;
    }

    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = ACTION_CODE_BINGO_SYNC_STATE,
        .length  = sizeof(BingoSyncPayload_St)
    };

    u8 buf[512];
    memcpy(buf, &tlv, sizeof(tlv));
    memcpy(buf + sizeof(tlv), &payload, sizeof(payload));

    broadcast(instance, BROADCAST_ALL, ACTION_CODE_GAME_DATA, buf, sizeof(tlv) + sizeof(payload));
}

/**
    @brief Sends the 12 choice cards once when the card-choice phase begins.
*/
static void bingo_sendChoiceCards(BingoGame_St* game, GameInstance_St* instance, BroadcastMessage_Ft broadcast) {
    if (broadcast == NULL || instance == NULL || game == NULL) return;

    BingoChoiceCardsPayload_St payload;
    for (uint i = 0; i < 12; ++i) {
        memcpy(&payload.choiceCards[i], &game->layout.choiceCards[i].values, sizeof(Card_t));
    }

    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = ACTION_CODE_BINGO_SEND_CHOICE_CARDS,
        .length  = sizeof(BingoChoiceCardsPayload_St)
    };

    u8 buf[sizeof(tlv) + sizeof(payload)];
    memcpy(buf, &tlv, sizeof(tlv));
    memcpy(buf + sizeof(tlv), &payload, sizeof(payload));

    broadcast(instance, BROADCAST_ALL, ACTION_CODE_GAME_DATA, buf, sizeof(buf));
}

/**
    @brief Handles an incoming player action for the Bingo instance.

    @param instance     Owning GameInstance_St (use instance->gameState for BingoGame_St).
    @param playerId     Player who sent the action.
    @param action       Action code.
    @param payload      Payload data.
    @param len          Payload length.
    @param broadcast    Scoped broadcast function (must be called with the same instance).
*/
void bingo_onAction(GameInstance_St* instance, s32 playerID, u8 action, const void* payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (broadcast == NULL || instance == NULL) return;
    if (action != ACTION_CODE_GAME_DATA) return;
    if (len < sizeof(GameTLVHeader_St)) return;

    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->game_id != MINI_GAME_BINGO) return; 

    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);

    BingoGame_St* game = (BingoGame_St*) instance->gameState;
    serverBroadcastCallback = broadcast;
    currentInstance = instance;

    switch (realAction) {
        case ACTION_CODE_JOIN_GAME: {
            if (numPlayers >= maxPlayers) break;

            u32 slot = numPlayers++;
            playerColors[slot]      = (u8)(slot % 4);
            playersReady[slot]      = false;
            playerChosenCard[slot]  = MAX_PLAYER;   // not locked yet

            // Give each player a default random card on join (can be overridden)
            u32 randomCard = rand() % 12;
            memcpy(&players[slot], &game->layout.choiceCards[randomCard].values, sizeof(Card_t));
            cardOwners[randomCard] = (u8)slot;

            log_info("Player %d joined Bingo (slot %u, color %u)", playerID, slot, playerColors[slot]);

            u8 buf_ack[1024] = {0};
            GameTLVHeader_St tlv_ack = { 
                .game_id = MINI_GAME_BINGO, 
                .action = ACTION_CODE_JOIN_ACK, 
                .length = sizeof(s32) 
            };

            memcpy(buf_ack, &tlv_ack, sizeof(tlv_ack));
            memcpy(buf_ack + sizeof(tlv_ack), &game->clientID, sizeof(s32));
            broadcast(instance, playerID, ACTION_CODE_GAME_DATA, buf_ack, sizeof(tlv_ack) + sizeof(s32));
        } break;

        case ACTION_CODE_BINGO_SET_MAX_PLAYERS: {
            if (playerID != 0) break;   // only host (first joined)
            if (len < sizeof(ActionSetMaxPlayersPayload_St)) break;

            ActionSetMaxPlayersPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            if (p.maxPlayers >= MIN_PLAYER && p.maxPlayers <= MAX_PLAYER) {
                maxPlayers = p.maxPlayers;
                log_info("Host set maxPlayers to %u", maxPlayers);
            }
        } break;

        case ACTION_CODE_BINGO_PLAYER_READY: {
            if (playerID < 0 || (u32)playerID >= numPlayers) break;
            playersReady[playerID] = true;
            log_info("Player %d is ready", playerID);

            // Check if all players are ready
            bool allReady = (numPlayers >= MIN_PLAYER);
            for (u32 i = 0; i < numPlayers; ++i) {
                if (!playersReady[i]) {
                    allReady = false;
                    break;
                }
            }

            if (allReady) {
                gameStatus = BINGO_STATUS_WAITING_CARD_CHOICE;
                game->progress.scene = GAME_SCENE_CARD_CHOICE;
                bingo_sendChoiceCards(game, currentInstance, broadcast);
                log_info("All players ready - entering card choice phase");
            }
        } break;

        case ACTION_CODE_BINGO_CHOOSE_CARD: {
            if (game->clientID == -1) break;
            if (len < sizeof(ActionChooseCardPayload_St)) break;

            ActionChooseCardPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            if (p.cardIndex < 12 && (u32)playerID < numPlayers && cardOwners[p.cardIndex] == MAX_PLAYER) {
                u32 slot = (u32)playerID;
                memcpy(&players[slot], &game->layout.choiceCards[p.cardIndex].values, sizeof(Card_t));
                log_info("Player %d previewed card %u", playerID, p.cardIndex);
            }
        } break;

        case ACTION_CODE_BINGO_LOCK_CARD: {
            if (len < sizeof(ActionLockCardPayload_St)) break;
            if (gameStatus != BINGO_STATUS_WAITING_CARD_CHOICE) break;

            ActionLockCardPayload_St p;
            memcpy(&p, realPayload, sizeof(p));

            u32 slot = (u32)playerID;
            if (slot < numPlayers && p.cardIndex < 12 && cardOwners[p.cardIndex] == MAX_PLAYER) {
                cardOwners[p.cardIndex] = (u8)slot;
                playerChosenCard[slot]  = p.cardIndex;
                playersReady[slot]      = true;   // locking also marks ready
                log_info("Player %d locked card %u", playerID, p.cardIndex);
            }
        } break;

        case ACTION_CODE_BINGO_DAUB_SQUARE: {
            if (len < sizeof(ActionDaubSquarePayload_St)) break;
            if (gameStatus != BINGO_STATUS_PLAYING) break;

            ActionDaubSquarePayload_St p;
            memcpy(&p, realPayload, sizeof(p));
            u32 slot = (u32)playerID;
            if (slot < numPlayers &&
                bingo_isValidDaub(&game->currentCall, &players[slot], p.row, p.col)) {
                players[slot].daubs[p.row][p.col] = true;
            }
        } break;

        default: log_warn("Action code (%d) not Implemented", action);
    }

    // Launch check (after possible lock)
    if (gameStatus == BINGO_STATUS_WAITING_CARD_CHOICE) {
        bool allLocked = true;
        for (u32 i = 0; i < numPlayers; ++i) {
            if (playerChosenCard[i] == MAX_PLAYER) {
                allLocked = false;
                break;
            }
        }
        if (allLocked) {
            gameStatus = BINGO_STATUS_LAUNCHING;
            game->currentCall.timer = 2.0f;   // 2-second countdown
            log_info("All players locked their cards - launching Bingo in 2 seconds");
        }
    }

    bingo_serverBroadcastSync(game, currentInstance, broadcast);
}

void bingo_onTick(void* state) {
    BingoGame_St* game = (BingoGame_St*) state;
    if (gameStatus != BINGO_STATUS_PLAYING || !serverBroadcastCallback) return;

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
        if (bingo_hasBingo(&players[i])) {
            game->progress.scene         = GAME_SCENE_END;
            game->progress.resultMessage = "BINGO! Player wins!";
            gameStatus = BINGO_STATUS_ENDED;
            break;
        }
    }

    if (game->balls.remainingCount == 0) {
        game->progress.scene         = GAME_SCENE_END;
        game->progress.resultMessage = "No more balls - Game Over";
        gameStatus = BINGO_STATUS_ENDED;
    }

    bingo_serverBroadcastSync(game, currentInstance, serverBroadcastCallback);
}

void bingo_onPlayerLeave(void* state, s32 playerID) {
    UNUSED(state);
    
    if (playerID < 0 || (u32)playerID >= numPlayers) return;

    // Simple shift for demo
    for (u32 i = (u32)playerID; i < numPlayers - 1; ++i) {
        players[i]          = players[i + 1];
        playersReady[i]     = playersReady[i + 1];
        playerColors[i]     = playerColors[i + 1];
        playerChosenCard[i] = playerChosenCard[i + 1];
    }

    numPlayers--;
    log_info("Player %d left Bingo room (remaining: %u)", playerID, numPlayers);
}

void bingo_destroyInstance(void* state) {
    BingoGame_St* game = (BingoGame_St*) state;

    if (game) {
        free(game);
    }
}

GameServerInterface_St bingoServerInterface = {
    .game_name        = "Bingo",
    .createInstance   = bingo_createInstance,
    .onAction         = bingo_onAction,
    .onTick           = bingo_onTick,
    .onPlayerLeave    = bingo_onPlayerLeave,
    .destroyInstance  = bingo_destroyInstance
};