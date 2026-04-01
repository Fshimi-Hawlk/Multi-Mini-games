/**
 * @file card.c
 * @author i-Charlys
 * @date 2026-03-18
 * @brief Implementation of card and deck management functions using O(1)/O(N) array operations.
 */

#include "core/card.h"
#include "rand.h"
#include <string.h>

/**
 * @brief Adds a card to the "top" of the deck (end of the array).
 * @param d Pointer to the deck.
 * @param c The card to push.
 * @return 1 on success, 0 if full.
 */
int push_card(Deck* d, Card c) {
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
Card remove_at(Deck* d, int index) {
    if (!d || index < 0 || index >= d->size) 
        return (Card){CARD_BLACK, ZERO};

    Card c = d->cards[index];
    
    // Shift elements left to maintain contiguous array
    if (index < d->size - 1) {
        memmove(&d->cards[index], &d->cards[index + 1], (d->size - 1 - index) * sizeof(Card));
    }
    
    d->size--;
    return c;
}

/**
 * @brief Pops the top card from the deck (the last element).
 * @param d Pointer to the deck.
 * @return The top card.
 */
Card pop_card(Deck* d) {
    if (!d || d->size == 0) return (Card){CARD_BLACK, ZERO};
    return d->cards[--d->size]; 
}

/**
 * @brief Clears all cards from the deck.
 * @param d Pointer to the deck.
 */
void clear_deck(Deck* d) {
    if (d) d->size = 0;
}

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm in-place.
 * @param d Pointer to the deck.
 */
void shuffle_deck(Deck* d) {
    if (!d || d->size <= 1) return;

    for (int i = d->size - 1; i > 0; i--) {
        int j = prng_rand() % (i + 1);
        Card temp = d->cards[i];
        d->cards[i] = d->cards[j];
        d->cards[j] = temp;
    }
}

/**
 * @brief Internal function to perform a single riffle shuffle in-place using temporary buffers.
 * @param array Array of cards to shuffle.
 * @param count Number of cards in the array.
 */
static void _perform_single_riffle(Card* array, int count) {
    if (count <= 1) return;

    // Cut roughly in the middle, +/- 10%
    int variance = count / 10; 
    int cutPoint = (count / 2) + (variance > 0 ? (prng_rand() % (variance * 2 + 1)) - variance : 0);

    // Bounds safety
    if (cutPoint < 0) cutPoint = 0;
    if (cutPoint > count) cutPoint = count;

    Card leftPile[MAX_UNO_CARDS];
    Card rightPile[MAX_UNO_CARDS];

    memcpy(leftPile, array, cutPoint * sizeof(Card));
    memcpy(rightPile, array + cutPoint, (count - cutPoint) * sizeof(Card));

    int l = 0, r = 0, main_idx = 0;
    int sizeL = cutPoint;
    int sizeR = count - cutPoint;

    while (l < sizeL || r < sizeR) {
        int takeFromLeft;
        if (l >= sizeL) takeFromLeft = 0;
        else if (r >= sizeR) takeFromLeft = 1;
        else takeFromLeft = (prng_rand() % 2); 

        int clumpSize = (prng_rand() % 3) + 1; // Drop 1 to 3 cards at once

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
void human_shuffle_deck(Deck* d) {
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
void init_uno_deck(Deck* d) {
    if (!d) return;
    d->size = 0;

    for (int c = CARD_RED; c <= CARD_BLUE; c++) {
        push_card(d, (Card){c, ZERO}); // One 0 per color
        for (int v = ONE; v <= NINE; v++) {
            push_card(d, (Card){c, v}); 
            push_card(d, (Card){c, v}); // Two of 1-9
        }
        for (int i = 0; i < 2; i++) {
            push_card(d, (Card){c, SKIP});
            push_card(d, (Card){c, REVERSE});
            push_card(d, (Card){c, PLUS_TWO});
        }
    }
    for (int i = 0; i < 4; i++) {
        push_card(d, (Card){CARD_BLACK, JOKER});
        push_card(d, (Card){CARD_BLACK, PLUS_FOUR});
    }
}
