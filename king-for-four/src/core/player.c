/**
 * @file player.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of player management functions.
 */

#include "core/player.h"
#include <string.h>

/**
 * @brief Initializes a player's data.
 * @param p Pointer to the player.
 * @param id Unique ID.
 * @param name Nickname.
 */
void init_player(Player* p, int id, const char* name) {
    if (p == NULL) return; 

    // Eliminate memory garbage
    memset(p, 0, sizeof(Player));

    p->id = id;
    p->is_local = 1; // Default value

    if (name != NULL) {
        strncpy(p->name, name, sizeof(p->name) - 1);
    }
}

/**
 * @brief Transfers a card from the draw pile to the player's hand.
 * @param p Pointer to the player.
 * @param draw_pile Pointer to the draw pile.
 */
void draw_to_hand(Player* p, Deck* draw_pile) {
    if (!p || !draw_pile) return;
    
    if (draw_pile->size > 0) {
        Card c = pop_card(draw_pile);
        push_card(&(p->hand), c);
    } else {
        printf("Plus de cartes dans la pioche pour %s !\n", p->name);
    }
}
