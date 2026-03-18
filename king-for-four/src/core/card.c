/**
 * @file card.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of card and deck management functions.
 */

#include "../../include/core/card.h"

/**
 * @brief Adds a card to the head of the deck's linked list.
 * @param d Pointer to the deck.
 * @param c The card to push.
 */
void push_card(Deck* d, Card c) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) return;

    newNode->card = c;
    
    // insertion par double pointeur
    Node** pp = &(d->head);
    newNode->next = *pp;
    *pp = newNode;
    
    d->size++;
}

/**
 * @brief Removes and returns the card at the specified index.
 * @param d Pointer to the deck.
 * @param index Index of the card to remove.
 * @return The removed card, or a black zero card if the index is invalid or deck is empty.
 */
Card remove_at(Deck* d, int index) {
    if (index < 0 || d->head == NULL) 
        return (Card){CARD_BLACK, ZERO};

    Node** pp = &(d->head);
    for (int i = 0; i < index && *pp != NULL; i++) {
        pp = &((*pp)->next);
    }

    if (*pp == NULL) return (Card){CARD_BLACK, ZERO};

    Node* toDelete = *pp;
    Card c = toDelete->card;
    *pp = toDelete->next;

    free(toDelete);
    if (d->size > 0) d->size--;
    return c;
}

/**
 * @brief Clears all cards from the deck and frees memory.
 * @param d Pointer to the deck.
 */
void clear_deck(Deck* d) {
    while (d->head != NULL) {
        Node* tmp = d->head;
        d->head = d->head->next;
        free(tmp);
    }
    d->size = 0;
}

/**
 * @brief Pops the top card from the deck.
 * @param d Pointer to the deck.
 * @return The top card.
 */
Card pop_card(Deck* d) {
    return remove_at(d, 0); 
}

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm.
 * @param d Pointer to the deck.
 */
void shuffle_deck(Deck* d) {
    if (d->head == NULL || d->head->next == NULL) {
        if (d->head == NULL) d->size = 0;
        else d->size = 1;
        return;
    }

    int count = 0;
    Node* curr = d->head;
    while (curr) { count++; curr = curr->next; }
    d->size = count;

    Card* tempArray = malloc(sizeof(Card) * count);
    if (!tempArray) return;
    
    for (int i = 0; i < count; i++) tempArray[i] = pop_card(d);

    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = tempArray[i];
        tempArray[i] = tempArray[j];
        tempArray[j] = temp;
    }

    for (int i = 0; i < count; i++) push_card(d, tempArray[i]);
    free(tempArray);
}

/**
 * @brief Initializes a standard 108-card Uno deck.
 * @param d Pointer to the deck.
 */
void init_uno_deck(Deck* d) {
    d->head = NULL;
    d->size = 0;

    for (int c = CARD_RED; c <= CARD_BLUE; c++) {
        push_card(d, (Card){c, ZERO}); // Un seul 0 par couleur
        for (int v = ONE; v <= NINE; v++) {
            push_card(d, (Card){c, v}); push_card(d, (Card){c, v}); // Deux de chaque 1-9
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

/**
 * @brief Frees all resources associated with the deck.
 * @param d Pointer to the deck.
 */
void free_deck(Deck* d) {
    while (d->head) pop_card(d);
}

/**
 * @brief Internal function to perform a single riffle shuffle on an array of cards.
 * @param array Array of cards to shuffle.
 * @param count Number of cards in the array.
 */
void _perform_single_riffle(Card* array, int count) {
    // 1. La Coupe (Cut) : On coupe environ au milieu, avec une erreur de +/- 10%
    int variance = count / 10; 
    int cutPoint = (count / 2) + (rand() % (variance * 2 + 1)) - variance;

    // Création de deux tas temporaires
    Card* leftPile = malloc(sizeof(Card) * cutPoint);
    Card* rightPile = malloc(sizeof(Card) * (count - cutPoint));

    for (int i = 0; i < cutPoint; i++) leftPile[i] = array[i];
    for (int i = 0; i < count - cutPoint; i++) rightPile[i] = array[cutPoint + i];

    // 2. L'Entrelacement (Interlacing)
    int l = 0, r = 0, main = 0;
    int sizeL = cutPoint;
    int sizeR = count - cutPoint;

    while (l < sizeL || r < sizeR) {
        // Décision : Est-ce qu'on fait tomber des cartes de Gauche ou de Droite ?
        int takeFromLeft;
        
        if (l >= sizeL) takeFromLeft = 0; // Gauche vide
        else if (r >= sizeR) takeFromLeft = 1; // Droite vide
        else {
            takeFromLeft = (rand() % 2); 
        }

        // COMBIEN de cartes tombent d'un coup ? 
        int clumpSize = (rand() % 3) + 1; // 1 à 3 cartes

        for (int i = 0; i < clumpSize; i++) {
            if (takeFromLeft && l < sizeL) {
                array[main++] = leftPile[l++];
            } else if (!takeFromLeft && r < sizeR) {
                array[main++] = rightPile[r++];
            }
        }
    }

    free(leftPile);
    free(rightPile);
}

/**
 * @brief Shuffles the deck using a human-like riffle shuffle algorithm.
 * @param d Pointer to the deck.
 */
void human_shuffle_deck(Deck* d) {
    if (d->head == NULL || d->head->next == NULL) {
        if (d->head == NULL) d->size = 0;
        else d->size = 1;
        return;
    }

    int count = 0;
    Node* curr = d->head;
    while (curr) { count++; curr = curr->next; }
    d->size = count;

    // 1. Conversion Liste -> Tableau
    Card* tempArray = malloc(sizeof(Card) * count);
    if (!tempArray) return;
    for (int i = 0; i < count; i++) tempArray[i] = pop_card(d);

    // 2. On répète le mélange 5 fois (Standard humain pour un bon mélange)
    int numberOfShuffles = 5; 
    for (int k = 0; k < numberOfShuffles; k++) {
        _perform_single_riffle(tempArray, count);
    }

    // 3. Conversion Tableau -> Liste
    for (int i = 0; i < count; i++) push_card(d, tempArray[i]);
    
    free(tempArray);
    printf("Melange 'Humain' effectue (%d passes) !\n", numberOfShuffles);
}
