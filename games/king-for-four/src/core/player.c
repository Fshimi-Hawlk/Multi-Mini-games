/**
    @file player.c
    @author i-Charlys
    @date 2026-03-11
    @date 2026-04-14
    @brief Implementation of player management functions.
*/
#include "core/player.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Initializes a player's data.
 * @param p Pointer to the player.
 * @param id Unique ID.
 * @param name Nickname.
 */
void kingForFour_initPlayer(Player_St* p, int id, const char* name) {
    if (p == NULL) return; 

    // Eliminate memory garbage
    memset(p, 0, sizeof(Player_St));

    p->id = id;
    p->is_local = 1; // Default value

    if (name != NULL) {
        strncpy(p->name, name, sizeof(p->name) - 1);
    }
}

/**
 * @brief Transfers a card from the draw pile to the player's hand.
 * @param p Pointer to the player.
 * @param drawPile Pointer to the draw pile.
 */
void kingForFour_drawToHand(Player_St* p, Deck_St* drawPile) {
    if (!p || !drawPile) return;
    
    if (drawPile->size > 0) {
        Card_St c = kingForFour_popCard(drawPile);
        kingForFour_pushCard(&(p->hand), c);
    } else {
        printf("Plus de cartes dans la pioche pour %s !\n", p->name);
    }
}
