#include "player.h"
#include <string.h>

// Initialise un joueur
void init_player(Player* p, int id, const char* name) {
    p->id = id;
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0'; 
    
    p->hand.head = NULL;
    p->hand.size = 0;
    p->is_local = 1; 
}

// Fait piocher une carte 
void draw_to_hand(Player* p, Deck* draw_pile) {
    if (draw_pile->size > 0) {
        Card c = pop_card(draw_pile);
        push_card(&(p->hand), c);
    } else {
        printf("Plus de cartes dans la pioche pour %s !\n", p->name);
    }
}