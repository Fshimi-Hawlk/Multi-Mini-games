/**
    @file card.c
    @author i-Charlys
    @date 2026-03-11
    @date 2026-04-14
    @brief Implementation of card and deck management functions using O(1)/O(N) array operations.
*/
#include "core/card.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// --- Public Functions ---

/**
 * @brief Adds a card to the "top" of the deck (end of the array).
 * @param d Pointer to the deck.
 * @param c The card to push.
 * @return 1 on success, 0 if full.
 */
int kingForFour_pushCard(Deck_St* d, Card_St c) {
    if (!d || d->size >= MAX_UNO_CARDS) return 0;
    d->cards[d->size++] = c;
    return 1;
}

/**
 * @brief Removes and returns the card at the specified index.
 * Shifts subsequent cards left to close the gap.
 * @param d Pointer to the deck.
 * @param index Index of the card to remove.
 * @return The removed card, or a black zero card if invalid.
 */
Card_St kingForFour_removeAt(Deck_St* d, int index) {
    if (!d || index < 0 || index >= d->size) 
        return (Card_St){CARD_BLACK, ZERO};

    Card_St c = d->cards[index];
    
    // Shift elements left to maintain contiguous array
    if (index < d->size - 1) {
        memmove(&d->cards[index], &d->cards[index + 1], (d->size - 1 - index) * sizeof(Card_St));
    }
    
    d->size--;
    return c;
}

/**
 * @brief Pops the top card from the deck (the last element).
 * @param d Pointer to the deck.
 * @return The top card.
 */
Card_St kingForFour_popCard(Deck_St* d) {
    if (!d || d->size == 0) return (Card_St){CARD_BLACK, ZERO};
    return d->cards[--d->size]; 
}

/**
 * @brief Clears all cards from the deck.
 * @param d Pointer to the deck.
 */
void kingForFour_clearDeck(Deck_St* d) {
    if (d) d->size = 0;
}

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm in-place.
 * @param d Pointer to the deck.
 */
void kingForFour_shuffleDeck(Deck_St* d) {
    if (!d || d->size <= 1) return;

    for (int i = d->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card_St temp = d->cards[i];
        d->cards[i] = d->cards[j];
        d->cards[j] = temp;
    }
}

/**
 * @brief Internal function to perform a single riffle shuffle in-place using temporary buffers.
 * @param array Array of cards to shuffle.
 * @param count Number of cards in the array.
 */
static void _perform_single_riffle(Card_St* array, int count) {
    if (count <= 1) return;

    // Cut roughly in the middle, +/- 10%
    int variance = count / 10; 
    int cutPoint = (count / 2) + (variance > 0 ? (rand() % (variance * 2 + 1)) - variance : 0);

    // Bounds safety
    if (cutPoint < 0) cutPoint = 0;
    if (cutPoint > count) cutPoint = count;

    Card_St leftPile[MAX_UNO_CARDS];
    Card_St rightPile[MAX_UNO_CARDS];

    memcpy(leftPile, array, cutPoint * sizeof(Card_St));
    memcpy(rightPile, array + cutPoint, (count - cutPoint) * sizeof(Card_St));

    int l = 0, r = 0, main_idx = 0;
    int sizeL = cutPoint;
    int sizeR = count - cutPoint;

    while (l < sizeL || r < sizeR) {
        int takeFromLeft;
        if (l >= sizeL) takeFromLeft = 0;
        else if (r >= sizeR) takeFromLeft = 1;
        else takeFromLeft = (rand() % 2); 

        int clumpSize = (rand() % 3) + 1; // Drop 1 to 3 cards at once

        for (int i = 0; i < clumpSize; i++) {
            if (takeFromLeft && l < sizeL) {
                array[main_idx++] = leftPile[l++];
            } else if (!takeFromLeft && r < sizeR) {
                array[main_idx++] = rightPile[r++];
            }
        }
    }
}

/**
 * @brief Shuffles the deck using a human-like riffle shuffle algorithm.
 * @param d Pointer to the deck.
 */
void kingForFour_humanShuffleDeck(Deck_St* d) {
    if (!d || d->size <= 1) return;

    int numberOfShuffles = 5; 
    for (int k = 0; k < numberOfShuffles; k++) {
        _perform_single_riffle(d->cards, d->size);
    }
    printf("Melange 'Humain' effectue (%d passes) !\n", numberOfShuffles);
}

/**
 * @brief Initializes a standard 108-card Uno deck.
 * @param d Pointer to the deck.
 */
void kingForFour_initUnoDeck(Deck_St* d) {
    if (!d) return;
    d->size = 0;

    for (int c = CARD_RED; c <= CARD_BLUE; c++) {
        kingForFour_pushCard(d, (Card_St){c, ZERO}); // One 0 per color
        for (int v = ONE; v <= NINE; v++) {
            kingForFour_pushCard(d, (Card_St){c, v}); 
            kingForFour_pushCard(d, (Card_St){c, v}); // Two of 1-9
        }
        for (int i = 0; i < 2; i++) {
            kingForFour_pushCard(d, (Card_St){c, SKIP});
            kingForFour_pushCard(d, (Card_St){c, REVERSE});
            kingForFour_pushCard(d, (Card_St){c, PLUS_TWO});
        }
    }
    for (int i = 0; i < 4; i++) {
        kingForFour_pushCard(d, (Card_St){CARD_BLACK, JOKER});
        kingForFour_pushCard(d, (Card_St){CARD_BLACK, PLUS_FOUR});
    }
}
