/**
 * @file card.h
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Header file for card and deck management in the King for Four game.
 */

#ifndef CARD_H
#define CARD_H

/** Maximum number of cards in a standard Uno game. */
#define MAX_UNO_CARDS 108

/**
 * @enum CardColor_Et
 * @brief Represents the possible colors of a card.
 */
typedef enum { 
    CARD_RED = 0,    /**< Red color */
    CARD_YELLOW,     /**< Yellow color */
    CARD_GREEN,      /**< Green color */
    CARD_BLUE,       /**< Blue color */
    CARD_BLACK       /**< Black color (for special cards) */
} CardColor_Et;

/**
 * @enum CardValue_Et
 * @brief Represents the possible values or actions of a card.
 */
typedef enum {
    ZERO = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    SKIP,      /**< Skip the next player's turn */
    REVERSE,   /**< Reverse the direction of play */
    PLUS_TWO,  /**< Next player draws two cards and skips their turn */
    JOKER,     /**< Change the current color */
    PLUS_FOUR  /**< Next player draws four cards and skips their turn */
} CardValue_Et;

/**
 * @struct Card_St
 * @brief Represents a single playing card.
 */
typedef struct {
    CardColor_Et color; /**< Color of the card */
    CardValue_Et value; /**< Value or action of the card */
} Card_St;

/**
 * @struct Deck_St
 * @brief Represents a collection of cards (a deck or a hand).
 * Uses a fixed-size array to ensure cache-locality, zero fragmentation,
 * and completely eliminate memory leaks.
 */
typedef struct {
    Card_St cards[MAX_UNO_CARDS]; /**< Array of cards in the deck */
    int size;                  /**< Number of cards currently in the deck */
} Deck_St;

// --- PROTOTYPES ---

/**
 * @brief Adds a card to the top of the deck.
 * @param d Pointer to the deck.
 * @param c The card to add.
 * @return 1 on success, 0 if deck is full.
 */
int kingForFour_pushCard(Deck_St* d, Card_St c);

/**
 * @brief Removes and returns the top card from the deck.
 * @param d Pointer to the deck.
 * @return The card that was removed. Returns a {CARD_BLACK, ZERO} card if empty.
 */
Card_St kingForFour_popCard(Deck_St* d);

/**
 * @brief Removes and returns a card at a specific index in the deck.
 * @param d Pointer to the deck.
 * @param index The index of the card to remove.
 * @return The card that was removed. Returns a {CARD_BLACK, ZERO} card if invalid index.
 */
Card_St kingForFour_removeAt(Deck_St* d, int index);

/**
 * @brief Clears all cards from the deck.
 * @param d Pointer to the deck to clear.
 */
void kingForFour_clearDeck(Deck_St* d);

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm.
 * @param d Pointer to the deck to shuffle.
 */
void kingForFour_shuffleDeck(Deck_St* d);

/**
 * @brief Shuffles the deck using a human-like riffle shuffle.
 * @param d Pointer to the deck to shuffle.
 */
void kingForFour_humanShuffleDeck(Deck_St* d);

/**
 * @brief Initializes a standard 108-card Uno deck.
 * @param d Pointer to the deck to initialize.
 */
void kingForFour_initUnoDeck(Deck_St* d);

#endif // CARD_H
