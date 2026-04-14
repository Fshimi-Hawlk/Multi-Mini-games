/**
 * @file player.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Header file for player management in King for Four.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

/**
 * @struct Player_St
 * @brief Represents a player in the game.
 */
typedef struct {
    int id;               /**< Unique identifier for the player */
    char name[50];        /**< Player's nickname */
    Deck_St hand;            /**< Linked list of cards in the player's hand */
    int is_local;         /**< 1 if the player is local (host), 0 if remote (client) */
} Player_St;

// --- PROTOTYPES ---

/**
 * @brief Initializes a player with a given ID and name.
 * @param p Pointer to the player structure to initialize.
 * @param id Unique identifier.
 * @param name Player_St's nickname.
 */
void kingForFour_initPlayer(Player_St* p, int id, const char* name);

/**
 * @brief Makes a player draw a card from a deck and add it to their hand.
 * @param p Pointer to the player.
 * @param draw_pile Pointer to the deck to draw from.
 */
void kingForFour_drawToHand(Player_St* p, Deck_St* draw_pile);

#endif
