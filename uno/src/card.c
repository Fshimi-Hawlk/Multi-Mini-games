#include "card.h"

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
        return (Card){BLACK, ZERO};

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

    for (int c = RED; c <= BLUE; c++) {
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
        push_card(d, (Card){BLACK, JOKER});
        push_card(d, (Card){BLACK, PLUS_FOUR});
    }
}

// Libération de la mémoire
void free_deck(Deck* d) {
    while (d->head) pop_card(d);
}