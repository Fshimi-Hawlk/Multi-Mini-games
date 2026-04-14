/**
    @file card.h
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Header file for card and deck management in the King for Four game.
*/
#ifndef CARD_H
#define CARD_H

/**
    @brief Maximum number of cards in a standard Uno game.
*/
/**
    @brief Represents the possible colors of a card.
*/
/**
    @brief Represents the possible values or actions of a card.
*/
/**
    @brief Represents a single playing card.
*/
/**
    @brief Represents a collection of cards (a deck or a hand).

    Uses a fixed-size array to ensure cache-locality, zero fragmentation,
    and completely eliminate memory leaks.
*/
#define MAX_UNO_CARDS 108


typedef enum { 
    CARD_RED = 0,    
    CARD_YELLOW,     
    CARD_GREEN,      
    CARD_BLUE,       
    CARD_BLACK       
} CardColor_Et;


typedef enum {
    ZERO = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    SKIP,      
    REVERSE,   
    PLUS_TWO,  
    JOKER,     
    PLUS_FOUR  
} CardValue_Et;


typedef struct {
    CardColor_Et color; 
    CardValue_Et value; 
} Card_St;


typedef struct {
    Card_St cards[MAX_UNO_CARDS]; 
    int size;                  
} Deck_St;

// --- PROTOTYPES ---

/**
    @brief Adds a card to the top of the deck.

    @param[in,out] d Pointer to the deck.
    @param[in]     c The card to add.
    @return          1 on success, 0 if deck is full.
*/
int kingForFour_pushCard(Deck_St* d, Card_St c);

/**
    @brief Removes and returns the top card from the deck.

    @param[in,out] d Pointer to the deck.
    @return          The card that was removed. Returns a {CARD_BLACK, ZERO} card if empty.
*/
Card_St kingForFour_popCard(Deck_St* d);

/**
    @brief Removes and returns a card at a specific index in the deck.

    @param[in,out] d     Pointer to the deck.
    @param[in]     index The index of the card to remove.
    @return              The card that was removed. Returns a {CARD_BLACK, ZERO} card if invalid index.
*/
Card_St kingForFour_removeAt(Deck_St* d, int index);

/**
    @brief Clears all cards from the deck.

    @param[in,out] d Pointer to the deck to clear.
*/
void kingForFour_clearDeck(Deck_St* d);

/**
    @brief Shuffles the deck using the Fisher-Yates algorithm.

    @param[in,out] d Pointer to the deck to shuffle.
*/
void kingForFour_shuffleDeck(Deck_St* d);

/**
    @brief Shuffles the deck using a human-like riffle shuffle.

    @param[in,out] d Pointer to the deck to shuffle.
*/
void kingForFour_humanShuffleDeck(Deck_St* d);

/**
    @brief Initializes a standard 108-card Uno deck.

    @param[out]    d Pointer to the deck to initialize.
*/
void kingForFour_initUnoDeck(Deck_St* d);

#endif // CARD_H
