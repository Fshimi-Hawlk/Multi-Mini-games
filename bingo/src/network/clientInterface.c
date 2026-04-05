/**
    @file clientInterface.c
    @author Fshimi-Hawlk
    @date 2026-03-31
    @date 2026-03-31
    @brief Client-side network interface and local rendering/input for the Bingo mini-game.
  
    Contributors:
        - Fshimi-Hawlk:
            - Full network interface following project conventions
            - Local rendering + input using bingo drawing functions
            - Action sending + state synchronization via minimal payload
*/

#include "core/game.h"

#include "setups/app.h"
#include "setups/game.h"

#include "ui/app.h"
#include "ui/game.h"

#include "utils/globals.h"

#include "utils/debug.h"
#include "utils/geometry.h"
#include "utils/container.h"
#include "utils/mathUtils.h"

#include "networkInterface.h"
#include "systemSettings.h"
#include "logger.h"

#include "widgets/button.h"

#include "APIs/generalAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Action codes (must stay in sync with server)
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

#pragma pack(push, 1)
typedef struct {
    u8 cardIndex;   ///< 0..11 – index into layout.choiceCards
} ActionChooseCardPayload_St;

typedef struct {
    u8 cardIndex;   ///< 0..11 – the card the player is locking
} ActionLockCardPayload_St;

typedef struct {
    u8 row;
    u8 col;
} ActionDaubSquarePayload_St;

typedef struct {
    u8 maxPlayers;  ///< MIN_PLAYER..MAX_PLAYER
} ActionSetMaxPlayersPayload_St;

typedef struct {
    Card_t choiceCards[12];
} BingoChoiceCardsPayload_St;

/**
    @brief Minimal payload sent to clients for synchronization.
           Only fields that actually change during play are included.
*/
typedef struct {
    f32              timer;
    char             displayedText[32];
    GameScene_Et     scene;
    const char*      resultMessage;
    u8               numPlayers;
    u8               maxPlayers;
    u8               playerColors[MAX_PLAYER];   ///< 0-3 for visual distinction
    u8               cardOwners[12];             ///< MAX_PLAYER = free, otherwise player slot
} BingoSyncPayload_St;
#pragma pack(pop)

static BingoGame_St localGame;
static bool         assetsLoaded = false;
static f32          joinRetryTimer = 0.0f;
static bool         isHost = false;
static u8           selectedCardIndex = MAX_PLAYER;   ///< currently previewed card (MAX_PLAYER = none)
static u8           maxPlayers = MIN_PLAYER;

static TextButton_St readyButton = {0};

/**
    @brief Helper to send a game-specific action to the server.
*/
static void sendToServer(u8 action, const void* data, u16 len) {
    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = action,
        .length  = len
    };

    RUDPHeader_St header;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &header);

    u8 buffer[1024];
    u32 offset = 0;

    memcpy(buffer + offset, &header, sizeof(header)); offset += sizeof(header);
    memcpy(buffer + offset, &tlv,    sizeof(tlv));    offset += sizeof(tlv);
    if (len > 0 && data != NULL) {
        memcpy(buffer + offset, data, len);
        offset += len;
    }

    send(networkSocket, buffer, offset, 0);
}

void bingo_init(void) {
    if (!assetsLoaded) {
        bingo_initFonts();
        assetsLoaded = true;
    }

    memset(&localGame, 0, sizeof(localGame));

    // Base fields
    localGame.base.running = true;

    bingo_computeLayout(&localGame.layout);

    localGame.clientID = -1;
    joinRetryTimer     = 0.0f;
    isHost             = false;
    selectedCardIndex  = MAX_PLAYER;

    readyButton = (TextButton_St){
        .bounds    = {100.0f, (f32) GetScreenHeight() - 120.0f, 220.0f, 50.0f},
        .state     = WIDGET_STATE_NORMAL,
        .text      = "READY",
        .baseColor = GREEN,
        .roundness = 0.4f
    };

    log_info("Bingo client initialized");
}

void bingo_onData(s32 playerId, u8 action, const void* data, u16 len) {
    UNUSED(playerId);

    if (data == NULL) return;

    switch (action) {
        case ACTION_CODE_JOIN_ACK: {
            if (len < sizeof(s32) ) break;

            memcpy(&localGame.clientID, data, sizeof(s32) );
            isHost = (localGame.clientID == 0);
            log_info("Bingo client received internal ID: %d (isHost=%s)", localGame.clientID, boolStr(isHost));
        } break;

        case ACTION_CODE_BINGO_SEND_CHOICE_CARDS: {
            if (len < sizeof(BingoChoiceCardsPayload_St)) break;

            BingoChoiceCardsPayload_St payload;
            memcpy(&payload, data, sizeof(payload));

            for (uint i = 0; i < 12; ++i) {
                memcpy(&localGame.layout.choiceCards[i].values, &payload.choiceCards[i], sizeof(Card_t));
            }

            log_info("Received 12 choice cards from server");
        } break;

        case ACTION_CODE_BINGO_SYNC_STATE: {
            if (len < sizeof(BingoSyncPayload_St)) break;

            BingoSyncPayload_St payload;
            memcpy(&payload, data, sizeof(BingoSyncPayload_St));

            localGame.currentCall.timer = payload.timer;
            
            strncpy(
                localGame.currentCall.displayedText, 
                payload.displayedText,
                sizeof(localGame.currentCall.displayedText) - 1
            );

            localGame.currentCall.displayedText[sizeof(localGame.currentCall.displayedText)-1] = '\0';

            localGame.progress.scene         = payload.scene;
            localGame.progress.resultMessage = payload.resultMessage;
        } break;

        default: log_warn("Action code (%d) not Implemented", action);
    }
}

void bingo_update(f32 dt) {
    if (localGame.clientID == -1) {
        joinRetryTimer += dt;
        if (joinRetryTimer > 1.0f) {
            sendToServer(ACTION_CODE_JOIN_GAME, NULL, 0);
            joinRetryTimer = 0.0f;
        }

        return;
    }

    f32Vector2 mouse = GetMousePosition();

    switch (localGame.progress.scene) {
        case GAME_SCENE_WAITING_ROOM: {
            if (textButtonUpdate(&readyButton, mouse)) {
                sendToServer(ACTION_CODE_BINGO_PLAYER_READY, NULL, 0);
            }

            if (isHost) {
                if (IsKeyPressed(KEY_UP)) {
                    ActionSetMaxPlayersPayload_St p = { .maxPlayers = (u8) clamp((s32) maxPlayers + 1, MIN_PLAYER, MAX_PLAYER) };
                    sendToServer(ACTION_CODE_BINGO_SET_MAX_PLAYERS, &p, sizeof(p));
                }
                if (IsKeyPressed(KEY_DOWN)) {
                    ActionSetMaxPlayersPayload_St p = { .maxPlayers = (u8) clamp((s32) maxPlayers - 1, MIN_PLAYER, MAX_PLAYER) };
                    sendToServer(ACTION_CODE_BINGO_SET_MAX_PLAYERS, &p, sizeof(p));
                }
            }
        } break;

        case GAME_SCENE_CARD_CHOICE: {
            for (uint i = 0; i < 12; ++i) {
                CardUI_St* card = &localGame.layout.choiceCards[i];
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                    CheckCollisionPointRec(mouse, card->backgroundRect)) {

                    if (selectedCardIndex != MAX_PLAYER) {
                        localGame.layout.choiceCards[selectedCardIndex].selected = false;
                    }

                    selectedCardIndex = (u8) i;
                    card->selected = true;

                    ActionChooseCardPayload_St payload = { .cardIndex = (u8) i };
                    sendToServer(ACTION_CODE_BINGO_CHOOSE_CARD, &payload, sizeof(payload));
                    break;
                }
            }

            // Lock card with ENTER
            if (selectedCardIndex != MAX_PLAYER && IsKeyPressed(KEY_ENTER)) {
                ActionLockCardPayload_St payload = { .cardIndex = selectedCardIndex };
                sendToServer(ACTION_CODE_BINGO_LOCK_CARD, &payload, sizeof(payload));
            }
        } break;

        case GAME_SCENE_PLAYING: {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bool inGrace = (localGame.currentCall.timer <= (localGame.balls.showDelay + localGame.balls.graceDelay));

                bool handled = false;
                for (u8 r = 0; r < 5 && !handled; ++r) {
                    for (u8 c = 0; c < 5 && !handled; ++c) {
                        Rectangle rect = {
                            localGame.layout.cardRectsRect.x + localGame.layout.cardRectsRect.width  * c,
                            localGame.layout.cardRectsRect.y + localGame.layout.cardRectsRect.height * r,
                            localGame.layout.cardRectsRect.width  - 1,
                            localGame.layout.cardRectsRect.height - 1
                        };

                        if (!CheckCollisionPointRec(mouse, rect)) continue;

                        if (inGrace && bingo_isValidDaub(&localGame.currentCall, &localGame.player, r, c)) {
                            localGame.player.daubs[r][c] = true;
                            localGame.player.misclicks[r][c] = 0;

                            ActionDaubSquarePayload_St payload = { .row = r, .col = c };
                            sendToServer(ACTION_CODE_BINGO_DAUB_SQUARE, &payload, sizeof(payload));
                        }
                        handled = true;
                    }
                }
            }
        } break;

        default: break;
    }
}

void bingo_draw(void) {
    if (!assetsLoaded) return;

    switch (localGame.progress.scene) {
        case GAME_SCENE_WAITING_ROOM: {
            if (localGame.clientID == -1) {
                drawTextPro("Connexion au serveur...", bingo_fonts[FONT28], localGame.layout.windowCenter, ANCHOR_CENTER, GRAY);
                break;
            }

            drawTextPro("Bingo - Waiting Room", bingo_fonts[FONT32], (f32Vector2) { .x = WINDOW_WIDTH / 2.0f, .y = 125.0f }, ANCHOR_CENTER, GOLD);

            textButtonDraw(&readyButton, bingo_fonts[FONT24], 24.0f);

            if (isHost) {
                drawTextPro(
                    TextFormat("Max players: %u", maxPlayers), 
                    bingo_fonts[FONT24], 
                    (f32Vector2) { .x = systemSettings.video.width - 100, .y = systemSettings.video.height - 120 }, 
                    ANCHOR_BOTTOM_RIGHT,
                    WHITE
                );
            }
        } break;

        case GAME_SCENE_CARD_CHOICE: bingo_drawChoiceCards(&localGame.layout); break;

        case GAME_SCENE_LAUNCHING: {
            bingo_drawChoiceCards(&localGame.layout);
            
            drawTextPro(
                TextFormat("%.0f", localGame.currentCall.timer / 2.0f), bingo_fonts[FONT128], 
                localGame.layout.windowCenter, ANCHOR_CENTER, BLACK
            );
        } break;

        case GAME_SCENE_PLAYING: {
            bingo_drawCard(&localGame.layout, &localGame.player);
            bingo_drawUI(&localGame.layout, &localGame.balls, &localGame.currentCall);
        } break;

        case GAME_SCENE_END: {
            drawTextPro(
                localGame.progress.resultMessage, 
                bingo_fonts[FONT64], 
                localGame.layout.windowCenter, 
                ANCHOR_CENTER, 
                (localGame.progress.resultMessage[0] == 'B') 
                    ? GREEN 
                    : RED
            );
        } break;
    }
}

GameClientInterface_St bingoClientInterface = {
    .id      = MINI_GAME_BINGO,
    .name    = "Bingo",
    .init    = bingo_init,
    .on_data = bingo_onData,
    .update  = bingo_update,
    .draw    = bingo_draw
};