/**
    @file protocol.h
    @author i-Charlys
    @date 2026-03-18
    @date 2026-04-14
    @brief Common network structures for the King-for-Four (Uno) module.
*/
#ifndef KING_PROTOCOL_H
#define KING_PROTOCOL_H

#include "core/card.h"
#include "baseTypes.h"

#pragma pack(push, 1)

/**
    @brief Payload for synchronizing game state from server to client.
*/
typedef struct {
    s32 current_player;     /**< Index of the current player (0-3) */
    s32 active_color;       /**< Current active color (-1:None, 0:Red, 1:Yellow, 2:Green, 3:Blue) */
    Card_St top_card;          /**< Card currently on top of the discard pile */
    s32 hand_sizes[4];      /**< Card count in each player's hand */
    s32 status;             /**< Game status (0: WAITING, 1: PLAYING, 2: GAME_OVER) */
    s32 host_id;            /**< ID of the host player (id 0) */
    s32 winner_id;          /**< ID of the winning player (-1 if none) */
} GameSyncPayload;

/**
    @brief Payload for an action to play a card.
*/
typedef struct {
    s32 card_index;         ///< Index of the card in the player's hand
    s32 chosen_color;       ///< Color chosen if playing a black card
} ActionPlayPayload_St;

#pragma pack(pop)

/** @brief Action code for playing a card. */
#define ACTION_PLAY_CARD (firstAvailableActionCode + 0x01)
/** @brief Action code for drawing a card. */
#define ACTION_DRAW_CARD (firstAvailableActionCode + 0x02)
/** @brief Action code for synchronizing the game state. */
#define ACTION_SYNC_GAME (firstAvailableActionCode + 0x03)
/** @brief Action code for joining a game. */
#define ACTION_JOIN_GAME (firstAvailableActionCode + 0x04)
/** @brief Action code for starting a game. */
#define ACTION_START_GAME (firstAvailableActionCode + 0x05)
/** @brief Action code for synchronizing a player's hand. */
#define ACTION_SYNC_HAND (firstAvailableActionCode + 0x06)
/** @brief Action code for acknowledging a join request. */
#define ACTION_JOIN_ACK (firstAvailableActionCode + 0x07)
/** @brief Action code for quitting the game. */
#define ACTION_QUIT_GAME (firstAvailableActionCode + 0x08)
/** @brief Action code for game over. */
#define ACTION_GAME_OVER (firstAvailableActionCode + 0x09)

#endif // KING_PROTOCOL_H
