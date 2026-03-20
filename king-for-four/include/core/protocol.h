/**
 * @file protocol.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Common network structures for the King-for-Four (Uno) module.
 */

#ifndef KING_PROTOCOL_H
#define KING_PROTOCOL_H

#include "core/card.h"

#pragma pack(push, 1)

/** @brief Payload for synchronizing game state from server to client. */
typedef struct {
    int current_player;     /**< Index of the current player (0-3) */
    int active_color;       /**< Current active color (-1:None, 0:Red, 1:Yellow, 2:Green, 3:Blue) */
    Card top_card;          /**< Card currently on top of the discard pile */
    int hand_sizes[4];      /**< Card count in each player's hand */
    int status;             /**< Game status (0: WAITING, 1: PLAYING) */
    int host_id;            /**< ID of the host player (id 0) */
} GameSyncPayload;

/** @brief Payload for an action to play a card. */
typedef struct {
    int card_index;         /**< Index of the card in the player's hand */
    int chosen_color;       /**< Color chosen if playing a black card */
} ActionPlayPayload_St;

#pragma pack(pop)

/** @brief Action code for playing a card. */
#define ACTION_PLAY_CARD 0x10
/** @brief Action code for drawing a card. */
#define ACTION_DRAW_CARD 0x11
/** @brief Action code for synchronizing the game state. */
#define ACTION_SYNC_GAME 0x12
/** @brief Action code for joining a game. */
#define ACTION_JOIN_GAME 0x13
/** @brief Action code for starting a game. */
#define ACTION_START_GAME 0x14
/** @brief Action code for synchronizing a player's hand. */
#define ACTION_SYNC_HAND 0x15
/** @brief Action code for acknowledging a join request. */
#define ACTION_JOIN_ACK 0x16
/** @brief Action code for quitting the game. */
#define ACTION_QUIT_GAME 0x17

#endif // KING_PROTOCOL_H
