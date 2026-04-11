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
#include "rudp_core.h"
#include "logger.h"

#include "APIs/generalAPI.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

// 
// Action codes (must stay in sync with server)
// 

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
    u32              numPlayers;
    u32              seed;
    char             resultMessage[64];
    PlayerCard_St    playerCards[MAX_PLAYER];
    s32              playerNetworkIds[MAX_PLAYER];
} BingoSyncPayload_St;
#pragma pack(pop)

static BingoGame_St localGame;
static bool         assetsLoaded = false;
static f32          joinRetryTimer = 0.0f;
static bool         cardsGenerated = false;

/**
    @brief Helper to send a game-specific action to the server.
*/
static void sendToServer(u8 action, const void* data, u16 len) {
    GameTLVHeader_St tlv = {
        .game_id = MINI_GAME_BINGO,
        .action  = action,
        .length  = htons(len)
    };

    RUDPHeader_St header;
    rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &header);
    header.sender_id = htons((u16)(localGame.clientID != -1 ? localGame.clientID : 0));

    u8 buffer[2048];
    memset(buffer, 0, sizeof(buffer));
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
    cardsGenerated     = false;

    // Ces valeurs sont des constantes de config partagées avec le serveur.
    // Sans elles, bingo_drawUI et inGrace ne fonctionnent pas en mode réseau
    // car memset les a mis à 0.
    localGame.balls.choiceDelay = 3.5f;
    localGame.balls.showDelay   = 1.5f;
    localGame.balls.graceDelay  = 1.0f;

    log_info("Bingo client initialized");
}

void bingo_onData(s32 playerId, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (playerId < 0 || (playerId >= MAX_CLIENTS && playerId != 999)) {
            log_warn("Bingo: Received data from invalid player ID: %d", playerId);
            return;
        }
    }

    if (data == NULL) return;

    switch (action) {
        case ACTION_CODE_JOIN_ACK: {
            if (len < sizeof(u16)) break;

            u16 net_id;
            memcpy(&net_id, data, sizeof(u16));
            localGame.clientID = (s32)ntohs(net_id);
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

            // Generate choice cards locally if not done yet (using shared seed).
            // Must happen BEFORE the player card sync so card->values are valid
            // when we fall back to copying them for the player's numbers.
            if (!cardsGenerated && payload.seed != 0) {
                srand(payload.seed);
                uint available[100];
                for (uint i = 0; i < 100; ++i) available[i] = i;

                extern bool bingo_generateCard(Card_t card, uint *available, uint count);
                for (uint i = 0; i < 12; ++i) {
                    bingo_generateCard(localGame.layout.choiceCards[i].values, available, 100);
                }
                cardsGenerated = true;
                log_info("Bingo cards generated locally with seed %u", payload.seed);

                // If the player had already selected a card before the seed arrived,
                // their numbers were copied from all-zero values. Re-copy now.
                if (localGame.previouslySelectedCard != NULL) {
                    memcpy(localGame.player.numbers, localGame.previouslySelectedCard->values, sizeof(Card_t));
                    log_info("Bingo: retroactively synced player numbers from selected card");
                }
            }

            // Sync local player card state (daubs and misclicks) from server.
            // clientID == slot index (sent via JOIN_ACK); playerCards is indexed by slot.
            if (localGame.clientID >= 0 && localGame.clientID < MAX_PLAYER) {
                int slot = localGame.clientID;
                for (int r = 0; r < 5; r++) {
                    for (int c = 0; c < 5; c++) {
                        localGame.player.daubs[r][c]     = payload.playerCards[slot].daubs[r][c];
                        localGame.player.misclicks[r][c] = payload.playerCards[slot].misclicks[r][c];
                    }
                }
                // Sync numbers from server if choice cards aren't generated yet locally
                if (!cardsGenerated) {
                    for (int r = 0; r < 5; r++) {
                        for (int c = 0; c < 5; c++) {
                            localGame.player.numbers[r][c] = payload.playerCards[slot].numbers[r][c];
                        }
                    }
                }
            }

            extern void updateWaitingRoomInfo(int players, int max, bool host);
            updateWaitingRoomInfo((int)payload.numPlayers, 4, (localGame.clientID == 0));

            strncpy(localGame.progress.resultMessage, payload.resultMessage, 63);

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

// Initialize the ball system for solo play (no server)
static void bingo_initBallsSolo(void) {
    uint b = 0;
    for (uint n = 0; n < 100; ++n)
        for (uint col = 1; col <= 5; ++col)
            localGame.balls.encodedBalls[b++] = 100 * col + n;
    shuffleArray(uint, localGame.balls.encodedBalls, 500, prng_rand);
    localGame.balls.remainingCount = 500;
    localGame.balls.choiceDelay    = 3.5f;
    localGame.balls.showDelay      = 1.5f;
    localGame.balls.graceDelay     = 1.0f;
}

void bingo_update(f32 dt) {
    // Solo mode (no server): give ourselves a local ID and init balls
    if (localGame.clientID == -1 && networkSocket < 0) {
        localGame.clientID = 0;
        bingo_initBallsSolo();
    }

    if (localGame.clientID == -1) {
        joinRetryTimer += dt;
        if (joinRetryTimer > 1.0f) {
            sendToServer(ACTION_CODE_JOIN_GAME, NULL, 0);
            joinRetryTimer = 0.0f;
        }
        return;
    }

    Vector2 mousePos = GetMousePosition();

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

                    // Copy the selected card's numbers into the player's card so
                    // bingo_isValidDaub has the right values during GAME_SCENE_PLAYING
                    memcpy(localGame.player.numbers, card->values, sizeof(Card_t));
                    memset(localGame.player.daubs, 0, sizeof(localGame.player.daubs));
                    localGame.player.daubs[2][2] = true; // free space

                    ActionChooseCardPayload_St payload = { .cardIndex = (u8)i };
                    sendToServer(ACTION_CODE_BINGO_CHOOSE_CARD, &payload, sizeof(payload));
                    break;
                }
            }

            if (localGame.previouslySelectedCard != NULL && IsKeyPressed(KEY_ENTER)) {
                if (networkSocket < 0) {
                    // Solo mode: transition locally without server
                    localGame.progress.scene = GAME_SCENE_LAUNCHING;
                    localGame.currentCall.timer = 6.5f;
                } else {
                    sendToServer(ACTION_CODE_BINGO_START_GAME, NULL, 0);
                }
            }
        } break;

        case GAME_SCENE_LAUNCHING: {
            // Solo mode: countdown locally then transition to PLAYING
            if (networkSocket < 0) {
                localGame.currentCall.timer -= dt;
                if (localGame.currentCall.timer < 0.0f) localGame.currentCall.timer = 0.0f;
                if (localGame.currentCall.timer <= 0.0f) {
                    localGame.currentCall.timer = localGame.balls.showDelay;
                    localGame.progress.scene = GAME_SCENE_PLAYING;
                }
            }
        } break;

        case GAME_SCENE_PLAYING: {
            // Solo mode: advance ball timer locally
            if (networkSocket < 0) {
                localGame.currentCall.timer += dt;
                if (localGame.currentCall.timer >= localGame.balls.choiceDelay) {
                    localGame.currentCall.timer = 0.0f;
                    if (localGame.balls.remainingCount > 0) {
                        localGame.currentCall.encodedValue = localGame.balls.encodedBalls[--localGame.balls.remainingCount];
                        localGame.currentCall.column = (localGame.currentCall.encodedValue / 100) - 1;
                        localGame.currentCall.number = localGame.currentCall.encodedValue - (localGame.currentCall.column + 1) * 100;
                        snprintf(localGame.currentCall.displayedText, sizeof(localGame.currentCall.displayedText),
                                 "%s %u", LETTERS[localGame.currentCall.column], localGame.currentCall.number);
                    }
                }
                if (bingo_hasBingo(&localGame.player)) {
                    localGame.progress.scene = GAME_SCENE_END;
                    strncpy(localGame.progress.resultMessage, "BINGO! You win!", 63);
                } else if (localGame.balls.remainingCount == 0) {
                    localGame.progress.scene = GAME_SCENE_END;
                    strncpy(localGame.progress.resultMessage, "No more balls - Game Over", 63);
                }
            }

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

                            if (networkSocket >= 0) {
                                ActionDaubSquarePayload_St payload = { .row = r, .col = c };
                                sendToServer(ACTION_CODE_BINGO_DAUB_SQUARE, &payload, sizeof(payload));
                            }
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
            if (localGame.previouslySelectedCard != NULL) {
                DrawText("APPUYEZ SUR ENTRÉE POUR COMMENCER", 
                         GetScreenWidth()/2 - MeasureText("APPUYEZ SUR ENTRÉE POUR COMMENCER", 20)/2, 
                         GetScreenHeight() - 50, 20, GREEN);
            }
            break;

        case GAME_SCENE_LAUNCHING:
            bingo_drawChoiceCards(&localGame.layout);
            char text[8] = {0};
            snprintf(text, sizeof(text), "%.0f", localGame.currentCall.timer / 2.0f);
            Vector2 textSize = MeasureTextEx(bingo_fonts[FONT48], text, 128, 0);
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

            Vector2 textPos = {
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