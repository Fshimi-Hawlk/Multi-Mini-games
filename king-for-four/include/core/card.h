/**
 * @file card.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Header file for card and deck management in the King for Four game.
 */

#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/** Maximum number of cards in a standard Uno game. */
#define MAX_UNO_CARDS 108

/**
 * @enum Card_Color
 * @brief Represents the possible colors of a card.
 */
typedef enum { 
    CARD_RED = 0,    /**< Red color */
    CARD_YELLOW,     /**< Yellow color */
    CARD_GREEN,      /**< Green color */
    CARD_BLUE,       /**< Blue color */
    CARD_BLACK       /**< Black color (for special cards) */
} Card_Color;

/**
 * @enum Card_Value
 * @brief Represents the possible values or actions of a card.
 */
typedef enum {
    ZERO = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    SKIP,      /**< Skip the next player's turn */
    REVERSE,   /**< Reverse the direction of play */
    PLUS_TWO,  /**< Next player draws two cards and skips their turn */
    JOKER,     /**< Change the current color */
    PLUS_FOUR  /**< Next player draws four cards and skips their turn */
} Card_Value;

/**
 * @struct Card
 * @brief Represents a single playing card.
 */
typedef struct {
    Card_Color color; /**< Color of the card */
    Card_Value value; /**< Value or action of the card */
} Card;

/**
 * @struct Deck
 * @brief Represents a collection of cards (a deck or a hand).
 * Uses a fixed-size array to ensure cache-locality, zero fragmentation,
 * and completely eliminate memory leaks.
 */
typedef struct {
    Card cards[MAX_UNO_CARDS]; /**< Array of cards in the deck */
    int size;                  /**< Number of cards currently in the deck */
} Deck;

// --- PROTOTYPES ---

/**
 * @brief Adds a card to the top of the deck.
 * @param d Pointer to the deck.
 * @param c The card to add.
 * @return 1 on success, 0 if deck is full.
 */
int push_card(Deck* d, Card c);

/**
 * @brief Removes and returns the top card from the deck.
 * @param d Pointer to the deck.
 * @return The card that was removed. Returns a {CARD_BLACK, ZERO} card if empty.
 */
Card pop_card(Deck* d);

/**
 * @brief Removes and returns a card at a specific index in the deck.
 * @param d Pointer to the deck.
 * @param index The index of the card to remove.
 * @return The card that was removed. Returns a {CARD_BLACK, ZERO} card if invalid index.
 */
Card remove_at(Deck* d, int index);

/**
 * @brief Clears all cards from the deck.
 * @param d Pointer to the deck to clear.
 */
void clear_deck(Deck* d);

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm.
 * @param d Pointer to the deck to shuffle.
 */
void shuffle_deck(Deck* d);

/**
 * @brief Shuffles the deck using a human-like riffle shuffle.
 * @param d Pointer to the deck to shuffle.
 */
void human_shuffle_deck(Deck* d);

/**
 * @brief Initializes a standard 108-card Uno deck.
 * @param d Pointer to the deck to initialize.
 */
void init_uno_deck(Deck* d);

#endif // CARD_H
