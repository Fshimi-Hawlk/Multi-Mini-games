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
#include "utils/utils.h"

#include "networkInterface.h"
#include "logger.h"

#include "APIs/generalAPI.h"

// ─────────────────────────────────────────────────────────────────────────────
// Action codes (must stay in sync with server)
// ─────────────────────────────────────────────────────────────────────────────

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
    const char*      resultMessage;
} BingoSyncPayload_St;
#pragma pack(pop)

static BingoGame_St localGame;
static bool         assetsLoaded = false;
static f32          joinRetryTimer = 0.0f;

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

    log_info("Bingo client initialized");
}

void bingo_onData(s32 playerId, u8 action, const void* data, u16 len) {
    UNUSED(playerId);

    if (data == NULL) return;

    switch (action) {
        case ACTION_CODE_JOIN_ACK: {
            if (len < sizeof(s32)) break;

            memcpy(&localGame.clientID, data, sizeof(s32));
            log_info("Bingo client received internal ID: %d", localGame.clientID);
        } break;

        case ACTION_CODE_BINGO_SYNC_STATE: {
            if (len < sizeof(BingoSyncPayload_St)) {
                log_warn(
                    "Bingo sync payload too small: %u bytes (expected %zu)", 
                    len, sizeof(BingoSyncPayload_St)
                );

                break;
            }

            BingoSyncPayload_St payload;
            memcpy(&payload, data, sizeof(BingoSyncPayload_St));

            // Apply minimal sync payload
            localGame.balls.remainingCount = payload.remainingBalls;
            localGame.currentCall          = payload.currentCall;
            localGame.progress.scene       = payload.scene;
            localGame.progress.resultMessage = payload.resultMessage;

            // Keep local preview cards / player card choice (server only syncs shared state)
            // Daubs are client-authoritative for the player's own card (with server validation)
            log_debug(
                "Bingo sync received: scene=%d, remainingBalls=%u", 
                (s32)payload.scene, payload.remainingBalls
            );
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

    f32Vector2 mousePos = GetMousePosition();

    switch (localGame.progress.scene) {
        case GAME_SCENE_CARD_CHOICE: {
            for (uint i = 0; i < 12; ++i) {
                CardUI_St* card = &localGame.layout.choiceCards[i];
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
                    CheckCollisionPointRec(mousePos, card->backgroundRect)) {

                    if (localGame.previouslySelectedCard != NULL) {
                        localGame.previouslySelectedCard->selected = false;
                    }

                    localGame.previouslySelectedCard = card;
                    card->selected = true;

                    ActionChooseCardPayload_St payload = { .cardIndex = (u8)i };
                    sendToServer(ACTION_CODE_BINGO_CHOOSE_CARD, &payload, sizeof(payload));
                    break;
                }
            }

            if (localGame.previouslySelectedCard != NULL && IsKeyPressed(KEY_ENTER)) {
                sendToServer(ACTION_CODE_BINGO_START_GAME, NULL, 0);
            }
        } break;

        case GAME_SCENE_PLAYING: {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bool inGrace = (localGame.currentCall.timer <=
                               (localGame.balls.showDelay + localGame.balls.graceDelay));

                bool handled = false;
                for (u8 r = 0; r < 5 && !handled; ++r) {
                    for (u8 c = 0; c < 5 && !handled; ++c) {
                        Rectangle rect = {
                            localGame.layout.cardRectsRect.x + localGame.layout.cardRectsRect.width  * c,
                            localGame.layout.cardRectsRect.y + localGame.layout.cardRectsRect.height * r,
                            localGame.layout.cardRectsRect.width  - 1,
                            localGame.layout.cardRectsRect.height - 1
                        };

                        if (!CheckCollisionPointRec(mousePos, rect)) continue;

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
        case GAME_SCENE_CARD_CHOICE:
            bingo_drawChoiceCards(&localGame.layout);
            break;

        case GAME_SCENE_LAUNCHING:
            bingo_drawChoiceCards(&localGame.layout);
            char text[8] = {0};
            sprintf(text, "%.0f", localGame.currentCall.timer / 2.0f);
            f32Vector2 textSize = MeasureTextEx(bingo_fonts[FONT48], text, 128, 0);
            DrawTextEx(bingo_fonts[FONT48], text,
                        Vector2Subtract(localGame.layout.windowCenter,
                                        Vector2Scale(textSize, 0.5f)),
                        128, 0, BLACK);
            break;

        case GAME_SCENE_PLAYING:
            bingo_drawCard(&localGame.layout, &localGame.player);
            bingo_drawUI(&localGame.layout, &localGame.balls, &localGame.currentCall);
            break;

        case GAME_SCENE_END: {
            f32 fontSize = 64.0f;
            u32 w = MeasureText(localGame.progress.resultMessage, fontSize);
            Color col = (localGame.progress.resultMessage[0] == 'B') ? GREEN : RED;

            f32Vector2 textPos = {
                localGame.layout.windowCenter.x - w / 2.0f,
                localGame.layout.windowCenter.y - fontSize / 2.0f
            };
            DrawTextEx(bingo_fonts[FONT48], localGame.progress.resultMessage,
                        textPos, fontSize, 0, col);
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