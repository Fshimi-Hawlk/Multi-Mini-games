#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

typedef struct {
    int id;               // Identifiant unique 
    char name[50];        // Pseudo
    Deck hand;            // Liste chaînée des cartes en main
    int is_local;         // 1 host, 0 client
} Player;

void init_player(Player* p, int id, const char* name);
void draw_to_hand(Player* p, Deck* draw_pile);

#endif