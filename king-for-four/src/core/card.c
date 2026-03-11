#include "../../include/core/card.h"

// Ajout en tête
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

// Retrait d'une carte
Card remove_at(Deck* d, int index) {
    if (index < 0 || index >= d->size || d->head == NULL) 
        return (Card){CARD_BLACK, ZERO};

    Node** pp = &(d->head);
    for (int i = 0; i < index; i++) {
        pp = &((*pp)->next);
    }

    Node* toDelete = *pp;
    Card c = toDelete->card;
    *pp = toDelete->next;

    free(toDelete);
    d->size--;
    return c;
}

Card pop_card(Deck* d) {
    return remove_at(d, 0); 
}

// Mélange Fisher-Yates (chaine -> tableau -> chaine) et ne pas oublier srand(time(NULL)) dans le main
void shuffle_deck(Deck* d) {
    if (d->size < 2) return;

    int count = d->size;
    Card* tempArray = malloc(sizeof(Card) * count);
    
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

// Initialisation du jeu de 108 cartes
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

// Libération de la mémoire
void free_deck(Deck* d) {
    while (d->head) pop_card(d);
}

// Fonction interne pour effectuer un seul "Riffle Shuffle" sur un tableau
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
        // Si un tas est vide, on prend forcément l'autre.
        // Sinon, probabilité proportionnelle à la taille du tas restant 
        int takeFromLeft;
        
        if (l >= sizeL) takeFromLeft = 0; // Gauche vide
        else if (r >= sizeR) takeFromLeft = 1; // Droite vide
        else {
            // Probabilité simple
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

void human_shuffle_deck(Deck* d) {
    if (d->size < 2) return;

    // 1. Conversion Liste -> Tableau
    int count = d->size;
    Card* tempArray = malloc(sizeof(Card) * count);
    for (int i = 0; i < count; i++) tempArray[i] = pop_card(d);

    // 2. On répète le mélange 5 fois (Standard humain pour un bon mélange)
    // Moins de fois = mélange très mauvais (cartes qui se suivent encore)
    int numberOfShuffles = 5; 
    for (int k = 0; k < numberOfShuffles; k++) {
        _perform_single_riffle(tempArray, count);
    }

    // 3. Conversion Tableau -> Liste
    // On réempile dans l'ordre du tableau mélangé
    for (int i = 0; i < count; i++) push_card(d, tempArray[i]);
    
    free(tempArray);
    printf("Melange 'Humain' effectue (%d passes) !\n", numberOfShuffles);
}