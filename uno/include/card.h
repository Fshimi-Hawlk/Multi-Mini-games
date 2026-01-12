#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// --- STRUCTURES DES CARTES ---
typedef enum { RED, YELLOW, GREEN, BLUE, BLACK } Card_Color;

typedef enum {
    ZERO = 0, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    SKIP, REVERSE, PLUS_TWO, JOKER, PLUS_FOUR
} Card_Value;

typedef struct {
    Card_Color color;
    Card_Value value;
} Card;

typedef struct Node {
    Card card;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    int size;
} Deck;

// --- STRUCTURE DU JOUEUR ---
typedef struct {
    int id;
    char name[50];
    Deck hand;
} Player;

// --- PROTOTYPES ---
void push_card(Deck* d, Card c);
Card pop_card(Deck* d);
Card remove_at(Deck* d, int index);
void shuffle_deck(Deck* d);
void init_uno_deck(Deck* d);

#endif