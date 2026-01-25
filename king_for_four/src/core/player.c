#include "../../include/core/player.h"
#include <string.h>

// Initialise un joueur

void init_player(Player* p, int id, const char* name) {

    if (p == NULL) return; // verifie que le pointeur existe 

    //élimine les déchets mémoire (pour eviter les failles reseaux)
    memset(p, 0, sizeof(Player));

    p->id = id;
    p->is_local = 1; // Valeur par défaut

    if (name != NULL) {
        strncpy(p->name, name, sizeof(p->name) - 1);
        // Pas besoin de '\0' grace memset
    }
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

