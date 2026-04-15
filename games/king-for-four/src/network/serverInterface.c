/**
    @file serverInterface.c
    @author i-Charlys
    @date 2026-03-18
    @date 2026-04-14
    @brief Server-side module for the King-for-Four (Uno) game.
*/
#include "APIs/generalAPI.h"
#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "core/bot.h"

#include "networkInterface.h"

#include "sharedUtils/random.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

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
    int currentPlayer;     /**< Index of the current player */
    int activeColor;       /**< Current active color */
    Card_St topCard;          /**< Card_St on top of the discard pile */
    int hand_sizes[4];      /**< Number of cards in each player's hand */
    int status;             /**< Game status (0: WAITING, 1: PLAYING) */
    int hostId;            /**< ID of the host player */
    int lastPlayerId;     /**< ID of the last player who moved */
    int lastAction;        /**< 0: Play, 1: Draw */
    int numPlayers;        /**< Current number of players in room */
    int requestedPlayers;  /**< Number of players requested by host */
} GameSyncPayload;

/**
    @brief Payload for a player playing a card.
*/
typedef struct {
    int cardIndex;
    int chosenColor;
} ActionPlayPayload_St;
#pragma pack(pop)

/**
    @brief Internal state maintained by the server for a game instance.
*/
typedef struct {
/**
    @brief Initializes a game instance on the server.

    @return A pointer to the newly created KingServerState_St.
*/
KingForFourGameState_St state;    
    s32 roomId;        
    int status;         
    int requestedPlayers; 
    float botTimer;    
    float botTargetTime; 
    int lastPlayerId;
    int lastAction;
    BroadcastMessage_Ft broadcast; 
} KingServerState;


void* king_createInstance(void) {
    KingServerState* ks = calloc(1, sizeof(KingServerState));
    if (ks) {
        kingForFour_initGameLogic(&ks->state);
        kingForFour_initUnoDeck(&ks->state.drawPile);
        kingForFour_shuffleDeck(&ks->state.drawPile);
        ks->status = 0; // WAITING
        ks->requestedPlayers = 4;
        ks->botTimer = 0;
        ks->botTargetTime = 1.0f + randfloat() * 2.0f; 
        ks->lastPlayerId = -1;
        ks->lastAction = -1;
        ks->broadcast = NULL;
    }
    return ks;
}

/**
    @brief Broadcasts the current game state to all players in a room.

    @param[in,out] ks        Pointer to the server state.
    @param[in]     room_id   The ID of the room to sync.
    @param[in]     broadcast The broadcast function to use.
*/
static void broadcast_sync(KingServerState* ks, s32 roomId, BroadcastMessage_Ft broadcast) {
    if (!broadcast) return;
    KingForFourGameState_St* g = &ks->state;
    Card_St topCard = {CARD_BLACK, ZERO};
    if (g->discardPile.size > 0) {
        topCard = g->discardPile.cards[g->discardPile.size - 1];
    }

    GameSyncPayload sync;
    sync.currentPlayer = g->currentPlayer;
    sync.activeColor = g->activeColor;
    sync.topCard = topCard;
    sync.status = ks->status;
    sync.hostId = (g->numPlayers > 0) ? g->players[0].id : -1;
    sync.lastPlayerId = ks->lastPlayerId;
    sync.lastAction = ks->lastAction;
    sync.numPlayers = g->numPlayers;
    sync.requestedPlayers = ks->requestedPlayers;
    for (int i = 0; i < 4; i++) {
        sync.hand_sizes[i] = (i < g->numPlayers) ? g->players[i].hand.size : 0;
    }

    u8 buf[2048];
    memset(buf, 0, sizeof(buf));
    GameTLVHeader_St tlv_sync = { .gameId = MINI_GAME_ID_KING_FOR_FOUR, .action = ACTION_CODE_SYNC_GAME, .length = htons(sizeof(GameSyncPayload)) };
    memcpy(buf, &tlv_sync, sizeof(tlv_sync));
    memcpy(buf + sizeof(tlv_sync), &sync, sizeof(sync));
    broadcast(roomId, -1, ACTION_CODE_GAME_DATA, buf, (u16)(sizeof(tlv_sync) + sizeof(sync)));
    
    for (int i = 0; i < g->numPlayers; i++) {
        if (g->players[i].id < 0) continue; // Skip bots for hand sync
        int targetPlayerId = g->players[i].id;
        int handCount = g->players[i].hand.size;
        if (handCount <= 0) continue;
        
        Card_St* cards = malloc((size_t)handCount * sizeof(Card_St));
        if (!cards) continue;
        for (int j = 0; j < handCount; j++) {
            cards[j] = g->players[i].hand.cards[j];
        }
        
        GameTLVHeader_St tlv_hand = { .gameId = MINI_GAME_ID_KING_FOR_FOUR, .action = ACTION_CODE_KFF_SYNC_HAND, .length = htons((u16)(handCount * sizeof(Card_St))) };
        memset(buf, 0, sizeof(buf));
        memcpy(buf, &tlv_hand, sizeof(tlv_hand));
        memcpy(buf + sizeof(tlv_hand), cards, (size_t)handCount * sizeof(Card_St));
        broadcast(UNICAST, targetPlayerId, ACTION_CODE_GAME_DATA, buf, (u16)(sizeof(tlv_hand) + handCount * sizeof(Card_St)));
        free(cards);
    }
}

/**
 * @brief Processes client actions and broadcasts updates.
 */
void king_onAction(void *state, s32 roomId, s32 playerId, u8 action, const void *payload, u16 len, BroadcastMessage_Ft broadcast) {
    if (action != ACTION_CODE_GAME_DATA || len < sizeof(GameTLVHeader_St)) return;

    GameTLVHeader_St* tlv = (GameTLVHeader_St*)payload;
    if (tlv->gameId != MINI_GAME_ID_KING_FOR_FOUR) return; 
    
    u8 realAction = tlv->action;
    void* realPayload = (u8*)payload + sizeof(GameTLVHeader_St);
    u16 payloadLen = len - sizeof(GameTLVHeader_St);

    KingServerState* ks = (KingServerState*)state;
    ks->roomId = roomId;
    ks->broadcast = broadcast; 
    KingForFourGameState_St* g = &ks->state;

    int internalId = -1;
    for (int i = 0; i < g->numPlayers; i++) {
        if (g->players[i].id == playerId) {
            internalId = i;
            break;
        }
    }

    if (internalId == -1 && ks->status == 0 && g->numPlayers < 4) {
        internalId = g->numPlayers++;
        kingForFour_initPlayer(&g->players[internalId], playerId, "Joueur");
        printf("[KING] Nouveau joueur enregistré: %d (Slot %d)\n", playerId, internalId);
    }

    if (internalId != -1 && internalId >= 0 && internalId < g->numPlayers) {
        if (realAction == ACTION_CODE_JOIN_GAME) {
            u8 bufAck[64];
            memset(bufAck, 0, sizeof(bufAck));
            GameTLVHeader_St tlvAck = { .gameId = MINI_GAME_ID_KING_FOR_FOUR, .action = ACTION_CODE_JOIN_ACK, .length = htons(sizeof(u16)) };
            u16 netId = htons((u16)internalId);
            memcpy(bufAck, &tlvAck, sizeof(tlvAck));
            memcpy(bufAck + sizeof(tlvAck), &netId, sizeof(u16));
            broadcast(UNICAST, playerId, ACTION_CODE_GAME_DATA, bufAck, sizeof(tlvAck) + sizeof(u16));
        }
        else if (realAction == ACTION_CODE_START_GAME && internalId == 0 && ks->status == 0) {
            int total_requested = ks->requestedPlayers;
            if (payloadLen >= sizeof(int)) memcpy(&total_requested, realPayload, sizeof(int));
            if (total_requested < g->numPlayers) total_requested = g->numPlayers;
            if (total_requested > 4) total_requested = 4;

            while (g->numPlayers < total_requested) {
                int botIdx = g->numPlayers++;
                kingForFour_initPlayer(&g->players[botIdx], -(botIdx + 1), "Bot");
                printf("[KING] Bot ajouté (Slot %d)\n", botIdx);
            }

            kingForFour_distributeCards(g);
            ks->status = 1; 
            printf("[KING] Partie démarrée avec %d joueurs.\n", g->numPlayers);
            broadcast_sync(ks, roomId, broadcast);
            return; 
        }
        else if (realAction == ACTION_CODE_KFF_SET_PLAYER_COUNT && internalId == 0 && ks->status == 0) {
            if (payloadLen >= sizeof(int)) {
                int count;
                memcpy(&count, realPayload, sizeof(int));
                if (count >= 2 && count <= 4) {
                    ks->requestedPlayers = count;
                    printf("[KING] Host changed player count to %d\n", count);
                }
            }
        }
        else if (realAction == ACTION_CODE_KFF_PLAY_CARD && ks->status == 1 && internalId == g->currentPlayer) {
            if (payloadLen < sizeof(ActionPlayPayload_St)) return;
            ActionPlayPayload_St p;
            memcpy(&p, realPayload, sizeof(ActionPlayPayload_St));
            
            if (p.cardIndex >= 0 && p.cardIndex < g->players[internalId].hand.size && kingForFour_tryPlayCard(g, internalId, p.cardIndex)) {
                Card_St played = g->discardPile.cards[g->discardPile.size - 1];
                ks->lastPlayerId = internalId;
                ks->lastAction = 0; 
                if (played.color == CARD_BLACK) g->activeColor = p.chosenColor;
                
                // Win detection
                if (g->players[internalId].hand.size == 0) {
                    ks->status = 2; // Finished
                    printf("[KING] Joueur %d a GAGNÉ !\n", internalId);
                }

                int nextP = (g->currentPlayer + g->gameDirection + g->numPlayers) % g->numPlayers;
                int skip = 0;
                if (played.value == PLUS_TWO) {
                    for(int i=0; i<2; i++) kingForFour_playerDrawCard(g, nextP);
                    skip = 1;
                } else if (played.value == PLUS_FOUR) {
                    for(int i=0; i<4; i++) kingForFour_playerDrawCard(g, nextP);
                    skip = 1;
                } else if (played.value == SKIP) skip = 1;
                else if (played.value == REVERSE) {
                    if (g->numPlayers == 2) skip = 1;
                    else g->gameDirection *= -1;
                }
                int step = g->gameDirection * (1 + skip);
                g->currentPlayer = (g->currentPlayer + step + g->numPlayers) % g->numPlayers;
            }
        } else if (realAction == ACTION_CODE_KFF_DRAW_CARD && ks->status == 1 && internalId == g->currentPlayer) {
            kingForFour_playerDrawCard(g, internalId);
            ks->lastPlayerId = internalId;
            ks->lastAction = 1; 
            g->currentPlayer = (g->currentPlayer + g->gameDirection + g->numPlayers) % g->numPlayers;
        } else if (realAction == ACTION_CODE_QUIT_GAME) {
            printf("[KING] Joueur %d quitte la room.\n", playerId);
        }
    }

    broadcast_sync(ks, roomId, broadcast);
}

/**
    @brief Updates the game state periodically (handles bot logic).

    @param[in,out] state Pointer to the KingServerState_St instance.
*/
void king_onTick(void* state) {
    KingServerState* ks = (KingServerState*)state;
    if (ks->status != 1 || !ks->broadcast) return;

    KingForFourGameState_St* g = &ks->state;
    int cp = g->currentPlayer;

    if (g->players[cp].id < 0) { // C'est un bot
        ks->botTimer += 0.016f; // Simulated delta
        if (ks->botTimer > ks->botTargetTime) { 
            ks->botTimer = 0;
            ks->botTargetTime = 1.0f + randfloat() * 1.5f; 
            
            int cardIdx = -1;
            kingForFour_calculateBestMove(g, cp, &cardIdx);
            
            if (cardIdx != -1) {
                if (cardIdx < g->players[cp].hand.size) {
                    Card_St toPlay = g->players[cp].hand.cards[cardIdx];
                    if (kingForFour_tryPlayCard(g, cp, cardIdx)) {
                        ks->lastPlayerId = cp;
                        ks->lastAction = 0; 
                        if (toPlay.color == CARD_BLACK) g->activeColor = (int)(rand() % 4);
                        
                        // Bot win detection
                        if (g->players[cp].hand.size == 0) {
                            ks->status = 2; // Finished
                            printf("[KING] Bot %d a GAGNÉ !\n", cp);
                        }

                        int nextP = (g->currentPlayer + g->gameDirection + g->numPlayers) % g->numPlayers;
                        int skip = 0;
                        if (toPlay.value == PLUS_TWO) {
                            for(int i=0; i<2; i++) kingForFour_playerDrawCard(g, nextP);
                            skip = 1;
                        } else if (toPlay.value == PLUS_FOUR) {
                            for(int i=0; i<4; i++) kingForFour_playerDrawCard(g, nextP);
                            skip = 1;
                        } else if (toPlay.value == SKIP) skip = 1;
                        else if (toPlay.value == REVERSE) {
                            if (g->numPlayers == 2) skip = 1;
                            else g->gameDirection *= -1;
                        }
                        int step = g->gameDirection * (1 + skip);
                        g->currentPlayer = (g->currentPlayer + step + g->numPlayers) % g->numPlayers;
                    }
                }
            } else {
                kingForFour_playerDrawCard(g, cp);
                ks->lastPlayerId = cp;
                ks->lastAction = 1; 
                g->currentPlayer = (g->currentPlayer + g->gameDirection + g->numPlayers) % g->numPlayers;
            }
            broadcast_sync(ks, ks->roomId, ks->broadcast);
        }
    }
}

/**
    @brief Handles a player leaving the game.

    @param[in,out] state     Pointer to the KingServerState_St instance.
    @param[in]     player_id The ID of the player who left.
*/
void king_onPlayerLeave(void* state, s32 playerId) {
    KingServerState* ks = (KingServerState*)state;
    KingForFourGameState_St* g = &ks->state;
    int internalId = -1;
    for (int i = 0; i < g->numPlayers; i++) {
        if (g->players[i].id == playerId) { internalId = i; break; }
    }
    if (internalId != -1) {
        printf("[KING] Joueur %d a quitté.\n", playerId);
        if (ks->status == 1) {
            g->players[internalId].id = -(internalId + 1);
            strncpy(g->players[internalId].name, "Bot (ex-humain)", sizeof(g->players[internalId].name) - 1);
            g->players[internalId].name[sizeof(g->players[internalId].name) - 1] = '\0';
        } else {
            for (int i = internalId; i < g->numPlayers - 1; i++) g->players[i] = g->players[i+1];
            g->numPlayers--;
        }
        if (ks->broadcast) broadcast_sync(ks, ks->roomId, ks->broadcast);
    }
}

/**
    @brief Destroys a game instance and frees memory.

    @param[in,out] state Pointer to the KingServerState_St instance to destroy.
*/
void king_destroyInstance(void *state) {
    KingServerState* ks = (KingServerState*)state;
    for (int i = 0; i < ks->state.numPlayers; i++) {
        kingForFour_clearDeck(&ks->state.players[i].hand);
    }
    kingForFour_clearDeck(&ks->state.drawPile);
    kingForFour_clearDeck(&ks->state.discardPile);
    free(ks);
}

GameServerInterface_St kingServerInterface = {
    .gameName          = "king-for-four",
    .createInstance    = king_createInstance,
    .onAction          = king_onAction,
    .onTick            = king_onTick, 
    .onPlayerLeave     = king_onPlayerLeave,
    .destroyInstance   = king_destroyInstance
};
