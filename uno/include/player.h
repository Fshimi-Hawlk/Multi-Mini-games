#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

typedef struct {
    int id;               // Identifiant unique (ex: 0, 1, 2, 3)
    char name[50];        // Pseudo
    Deck hand;            // Liste chaînée des cartes en main
    int is_local;         // 1 si c'est toi, 0 si c'est un joueur distant (utile pour le réseau)
} Player;

void init_player(Player* p, int id, const char* name);
void draw_to_hand(Player* p, Deck* draw_pile);

#endif