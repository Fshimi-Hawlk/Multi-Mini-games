/**
 * @file kingClient_module.c
 * @author i-Charlys
 * @author Fshimi-Hawlk
 * @date 2026-03-18
 * @date 2026-03-30
 * @brief Client-side module for the King-for-Four game, handling network synchronization and UI.
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>

#include "core/game.h"
#include "ui/renderer.h"
#include "networkInterface.h"
#include "logger.h"
#include "APIs/generalAPI.h"

/**
    @brief Action codes for King-for-Four specific network messages.
*/
enum {
    ACTION_CODE_KFF_PLAY_CARD = firstAvailableActionCode, ///< Play a card from hand
    ACTION_CODE_KFF_DRAW_CARD,                            ///< Draw a card from the deck
    ACTION_CODE_KFF_SYNC_HAND,                            ///< Synchronize the player's hand
    ACTION_CODE_KFF_SET_PLAYER_COUNT,                     ///< Set the number of players (host only)
};

#pragma pack(push, 1)
/**
    @brief Payload for synchronizing game state from server to client.
*/
typedef struct {
    int currentPlayer;
    int activeColor;
    Card_St topCard;
    int handSizes[4];
    int status;
    int hostId;
    int lastPlayerId;
    int last_action;
    int numPlayers;
    int requestedPlayers;
} GameSyncPayload_St;

/**
    @brief Payload for a player playing a card.
*/
typedef struct {
    int cardIndex;
    int chosenColor;
} ActionPlayPayload_St;
#pragma pack(pop)
static KingForFourGameState_St kingForFour_localState;
static GameAssets_St assets;
static bool assets_loaded = false;
static int myInternalId = -1;
static int gameStatus = 0;
static float join_retry_timer = 0;

static float turn_overlay_timer = 0;
static int winnerId = -1;
static float last_move_timer = 0;
static int lastPlayer_who_moved = -1;
static int last_action_type = -1;
static Card_St lastSeen_topCard = {CARD_BLACK, ZERO};
static float cardPop_timer = 0;
static bool isChoosingColor = false;
static int pendingCardIndex = -1;
static bool showInfo_window = false;

static void send_toServer(u8 action, void* data, u16 len) {
    GameTLVHeader_St tlv = { .gameId = MINI_GAME_ID_KING_FOR_FOUR, .action = action, .length = htons(len) };
    RUDPHeader_St h; rudpGenerateHeader(&serverConnection, ACTION_CODE_GAME_DATA, &h);
    h.senderId = htons((u16)(myInternalId != -1 ? myInternalId : 0));
    u8 buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, &h, sizeof(h));
    memcpy(buffer + sizeof(h), &tlv, sizeof(tlv));
    if (len > 0 && data) memcpy(buffer + sizeof(h) + sizeof(tlv), data, len);
    send(networkSocket, buffer, sizeof(h) + sizeof(tlv) + len, 0);
}

/**
    @brief Initializes the client-side game state and assets.
*/
void kingClientInit(void) {
    if (!assets_loaded) {
        assets = kingForFour_loadAssets();
        assets_loaded = true;
    }
    memset(&kingForFour_localState, 0, sizeof(KingForFourGameState_St));
    kingForFour_initGameLogic(&kingForFour_localState);
    myInternalId = -1;
    gameStatus = 0;
    winnerId = -1;
    join_retry_timer = 0;
    isChoosingColor = false;
    pendingCardIndex = -1;
    showInfo_window = false;
}

static int selectedPlayers = 4;

/**
    @brief Callback for handling data received from the server.

    @param[in] player_id ID of the sender (server is usually 0 or special).
    @param[in] action    The action code of the message.
    @param[in] data      Pointer to the received data.
    @param[in] len       Length of the received data.
*/
void kingClient_on_data(s32 playerId, u8 action, const void* data, u16 len) {
    if (action != ACTION_CODE_JOIN_ACK) {
        if (playerId < 0 || (playerId >= MAX_CLIENTS && playerId != 999)) {
            log_warn("[KING] Data received from invalid player ID: %d", playerId);
            return;
        }
    }
    if (data == NULL) return;
    if (action == ACTION_CODE_JOIN_ACK) {
        if (len >= sizeof(u16)) {
            u16 netId;
            memcpy(&netId, data, sizeof(u16));
            myInternalId = (int)ntohs(netId);
            log_info("[KING] Mon ID interne: %d", myInternalId);
        }
    } else if (action == ACTION_CODE_SYNC_GAME) {
        if (len >= (u16) sizeof(GameSyncPayload_St)) {
            GameSyncPayload_St sync;
            memcpy(&sync, data, sizeof(GameSyncPayload_St));

            selectedPlayers = sync.requestedPlayers;

            if (sync.currentPlayer != kingForFour_localState.currentPlayer && sync.currentPlayer == myInternalId) {
                turn_overlay_timer = 2.0f;
            }
            kingForFour_localState.currentPlayer = sync.currentPlayer;
            kingForFour_localState.activeColor = sync.activeColor;
            kingForFour_localState.numPlayers = sync.numPlayers;
            gameStatus = sync.status;

            extern void lobby_updateWaitingRoomInfo(int players, int max, bool host);
            lobby_updateWaitingRoomInfo(sync.numPlayers, 4, (myInternalId == 0));

            if (sync.lastPlayerId != -1) {
                lastPlayer_who_moved = sync.lastPlayerId;
                last_action_type = sync.last_action;
                last_move_timer = 1.5f;
                if (sync.topCard.color != lastSeen_topCard.color || sync.topCard.value != lastSeen_topCard.value) {
                    cardPop_timer = 0.5f;
                    lastSeen_topCard = sync.topCard;
                }
            }
            if (kingForFour_localState.discardPile.size == 0) kingForFour_pushCard(&kingForFour_localState.discardPile, sync.topCard);
            else kingForFour_localState.discardPile.cards[kingForFour_localState.discardPile.size - 1] = sync.topCard;
            
            for (int i = 0; i < sync.numPlayers; i++) {
                kingForFour_localState.players[i].hand.size = sync.handSizes[i];
            }
            if (gameStatus == 1) {
                for (int i = 0; i < sync.numPlayers; i++) {
                    if (sync.handSizes[i] == 0) winnerId = i;
                }
            }
        }
    } else if (action == ACTION_CODE_KFF_SYNC_HAND) {
        int count = len / sizeof(Card_St);
        if (myInternalId >= 0 && myInternalId < 4) {
            kingForFour_clearDeck(&kingForFour_localState.players[myInternalId].hand);
            for (int i = 0; i < count; i++) {
                Card_St c; memcpy(&c, (u8*)data + (i * sizeof(Card_St)), sizeof(Card_St));
                kingForFour_pushCard(&kingForFour_localState.players[myInternalId].hand, c);
            }
        }
    }
}

/**
    @brief Updates the client-side game logic and handles user input.

    @param[in] dt Delta time since the last frame.
*/
void kingClient_update(float dt) {
    if (!assets_loaded) return;
    if (myInternalId == -1) {
        join_retry_timer += dt;
        if (join_retry_timer > 1.0f) {
            send_toServer(ACTION_CODE_JOIN_GAME, NULL, 0);
            join_retry_timer = 0;
        }
    }
    if (turn_overlay_timer > 0) turn_overlay_timer -= dt;
    if (last_move_timer > 0) last_move_timer -= dt;
    if (cardPop_timer > 0) cardPop_timer -= dt;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 m = GetMousePosition();
        Rectangle infoIconRect = { (float)GetScreenWidth() - 40, 40, 30, 30 };
        if (CheckCollisionPointRec(m, infoIconRect)) {
            showInfo_window = !showInfo_window;
        } else if (showInfo_window) {
            showInfo_window = false; // Close when clicking elsewhere
        }
    }

    if (gameStatus == 1) {
        if (isChoosingColor) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 m = GetMousePosition();
                int sw = GetScreenWidth(); int sh = GetScreenHeight();
                Rectangle colors[4] = {
                    { sw / 2.0f - 100, sh / 2.0f - 100, 100, 100 }, { sw / 2.0f, sh / 2.0f - 100, 100, 100 },
                    { sw / 2.0f - 100, sh / 2.0f, 100, 100 }, { sw / 2.0f, sh / 2.0f, 100, 100 }
                };
                for (int i = 0; i < 4; i++) {
                    if (CheckCollisionPointRec(m, colors[i])) {
                        ActionPlayPayload_St payload = { .cardIndex = pendingCardIndex, .chosenColor = i };
                        send_toServer(ACTION_CODE_KFF_PLAY_CARD, &payload, sizeof(payload));
                        isChoosingColor = false;
                        break;
                    }
                }
            }
        }
        else if (kingForFour_localState.currentPlayer == myInternalId && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (myInternalId >= 0 && myInternalId < 4) {
                int clickedHandIndex = kingForFour_getHoveredCardIndex(&kingForFour_localState.players[myInternalId], assets);
                if (clickedHandIndex != -1) {
                    if (kingForFour_localState.players[myInternalId].hand.cards[clickedHandIndex].color == CARD_BLACK) {
                        isChoosingColor = true; pendingCardIndex = clickedHandIndex;
                    } else {
                        ActionPlayPayload_St payload = { .cardIndex = clickedHandIndex, .chosenColor = -1 };
                        send_toServer(ACTION_CODE_KFF_PLAY_CARD, &payload, sizeof(payload));
                    }
                } else if (CheckCollisionPointRec(GetMousePosition(), kingForFour_getDeckRect(assets))) {
                    send_toServer(ACTION_CODE_KFF_DRAW_CARD, NULL, 0);
                }
            }
        }
    }
    if (gameStatus == 0 && myInternalId == 0) {
        bool changed = false;
        if (IsKeyPressed(KEY_UP) && selectedPlayers < 4) { selectedPlayers++; changed = true; }
        if (IsKeyPressed(KEY_DOWN) && selectedPlayers > 2) { selectedPlayers--; changed = true; }
        
        if (changed) {
            send_toServer(ACTION_CODE_KFF_SET_PLAYER_COUNT, &selectedPlayers, sizeof(int));
        }

        if (IsKeyPressed(KEY_ENTER)) send_toServer(ACTION_CODE_START_GAME, &selectedPlayers, sizeof(int));
    }
    // removed local ESC handler to use lobby pause menu instead
}

/**
    @brief Renders the game screen for the client.
*/
void kingClient_draw(void) {
    if (!assets_loaded) return;
    if (gameStatus == 0) {
        DrawText("KING FOR FOUR - SALLE D'ATTENTE", 100, 100, 40, GOLD);
        if (myInternalId != -1) {
            DrawText(TextFormat("Vous êtes le JOUEUR %d", myInternalId), 100, 180, 30, WHITE);
            DrawText(TextFormat("Joueurs connectés : %d", kingForFour_localState.numPlayers), 100, 215, 22, LIGHTGRAY);
            if (myInternalId == 0) {
                DrawText(TextFormat("HÔTE: Flèches HAUT/BAS : %d JOUEURS (dont bots)", selectedPlayers), 100, 250, 25, GREEN);
                DrawText("Appuyez sur ENTRÉE pour lancer.", 100, 285, 25, GREEN);
            } else {
                DrawText(TextFormat("En attente de l'hôte... (%d JOUEURS)", selectedPlayers), 100, 250, 30, LIGHTGRAY);
            }
        } else DrawText("Connexion au serveur...", 100, 180, 30, GRAY);
        return;
    }
    kingForFour_renderTable(&kingForFour_localState, assets, cardPop_timer > 0 ? (cardPop_timer * 0.2f) : 0);
    kingForFour_renderOpponents(&kingForFour_localState, assets, myInternalId);
    if (myInternalId >= 0 && myInternalId < 4) {
        kingForFour_renderHand(&kingForFour_localState.players[myInternalId], assets);
    }
    if (last_move_timer > 0 && lastPlayer_who_moved != -1) {
        const char* pName = (lastPlayer_who_moved == myInternalId) ? "VOUS" : TextFormat("JOUEUR %d", lastPlayer_who_moved);
        const char* actionName = (last_action_type == 0) ? "a JOUÉ une carte" : "a PIOCHÉ une carte";
        float alpha = last_move_timer > 0.5f ? 1.0f : last_move_timer * 2.0f;
        DrawText(TextFormat("%s %s", pName, actionName), GetScreenWidth()/2 - 100, GetScreenHeight()/2 + 80, 20, Fade(GOLD, alpha));
    }
    if (kingForFour_localState.currentPlayer == myInternalId) {
        float pulse = (sinf(GetTime() * 10.0f) + 1.0f) * 0.5f;
        DrawText("C'EST VOTRE TOUR !", 10, 40, 25, Fade(GREEN, 0.5f + pulse * 0.5f));
    } else DrawText(TextFormat("Tour du Joueur %d", kingForFour_localState.currentPlayer), 10, 40, 25, YELLOW);
    if (kingForFour_localState.activeColor != -1) {
        Color c = RED; const char* name = "ROUGE";
        if (kingForFour_localState.activeColor == 1) { c = YELLOW; name = "JAUNE"; }
        else if (kingForFour_localState.activeColor == 2) { c = GREEN; name = "VERT"; }
        else if (kingForFour_localState.activeColor == 3) { c = BLUE; name = "BLEU"; }
        DrawText(TextFormat("COULEUR DEMANDÉE : %s", name), 10, 70, 20, c);
    }
    if (isChoosingColor) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        DrawRectangle(0, 0, sw, sh, Fade(BLACK, 0.7f));
        DrawText("CHOISISSEZ UNE COULEUR", sw / 2.0f - 150, sh / 2.0f - 150, 25, WHITE);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f - 100, 100, 100, RED);
        DrawRectangle(sw / 2.0f, sh / 2.0f - 100, 100, 100, YELLOW);
        DrawRectangle(sw / 2.0f - 100, sh / 2.0f, 100, 100, GREEN);
        DrawRectangle(sw / 2.0f, sh / 2.0f, 100, 100, BLUE);
    }
    if (turn_overlay_timer > 0) {
        float alpha = turn_overlay_timer > 1.0f ? 0.8f : turn_overlay_timer * 0.8f;
        DrawRectangle(0, GetScreenHeight()/2 - 60, GetScreenWidth(), 120, Fade(GOLD, alpha));
        DrawText("C'EST VOTRE TOUR", GetScreenWidth()/2 - MeasureText("C'EST VOTRE TOUR", 60)/2, GetScreenHeight()/2 - 30, 60, WHITE);
    }
    if (winnerId != -1) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.8f));
        const char* txt = (winnerId == myInternalId) ? "VICTOIRE !" : TextFormat("JOUEUR %d A GAGNÉ !", winnerId);
        DrawText(txt, GetScreenWidth()/2 - MeasureText(txt, 80)/2, GetScreenHeight()/2 - 100, 80, winnerId == myInternalId ? GREEN : RED);
        DrawText("Appuyez sur ESC pour quitter", GetScreenWidth()/2 - 150, GetScreenHeight()/2 + 50, 20, GRAY);
    }

    // Info Icon
    Rectangle infoIconRect = { (float)GetScreenWidth() - 40, 40, 30, 30 };
    bool hoverInfo = CheckCollisionPointRec(GetMousePosition(), infoIconRect);
    DrawCircleV((Vector2){infoIconRect.x + 15, infoIconRect.y + 15}, 15, hoverInfo ? SKYBLUE : BLUE);
    DrawText("i", (int)infoIconRect.x + 11, (int)infoIconRect.y + 5, 25, WHITE);

    if (showInfo_window) {
        int sw = GetScreenWidth(); int sh = GetScreenHeight();
        Rectangle win = { sw/2.0f - 250, sh/2.0f - 200, 500, 400 };
        DrawRectangleRec(win, Fade(DARKGRAY, 0.95f));
        DrawRectangleLinesEx(win, 2, GOLD);
        DrawText("POUVOIRS DES CARTES", (int)win.x + 120, (int)win.y + 20, 25, GOLD);
        
        int ty = (int)win.y + 70;
        DrawText("- SKIP (Symbole barré) : Passe le tour du suivant", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- REVERSE (Flèches) : Inverse le sens de jeu", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- +2 : Le suivant pioche 2 cartes et passe son tour", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- JOKER (Couleur changeante) : Change la couleur", (int)win.x + 30, ty, 18, WHITE); ty += 40;
        DrawText("- +4 : Le suivant pioche 4 cartes et passe son tour", (int)win.x + 30, ty, 18, WHITE); ty += 60;
        
        DrawText("Cliquez n'importe où pour fermer", (int)win.x + 100, (int)win.y + 360, 18, GRAY);
    }

    DrawText("ESC pour quitter", GetScreenWidth() - 150, 10, 15, GRAY);
}

GameClientInterface_St kingForFourClientInterface = {
    .id = MINI_GAME_ID_KING_FOR_FOUR,
    .name = "King For Four",
    .init = kingClientInit,
    .on_data = kingClient_on_data,
    .update = kingClient_update,
    .draw = kingClient_draw
};
