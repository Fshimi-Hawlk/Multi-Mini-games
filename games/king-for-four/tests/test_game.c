/**
    @file test_game.c
    @author i-Charlys
    @date 2026-03-11
    @date 2026-04-14
    @brief Unit tests and debug utilities for the King-for-Four game logic.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/core/game.h"

/**
 * @brief Prints a debug representation of a card to the console.
 * @param c The card to print.
 */
void print_card_debug(Card c) {
    const char *clrs[] = {"ROUGE", "JAUNE", "VERT", "BLEU", "NOIR"};
    const char *vals[] = {"0","1","2","3","4","5","6","7","8","9","SKIP","REVERSE","+2","JOKER","+4"};
    printf("[%s %s]", clrs[c.color], vals[c.value]);
}

/**
 * @brief Main entry point for the game logic tests.
 * @return 0 on completion.
 */
int main() {
    srand(time(NULL));
    GameState g = {0};
    g.num_players = 1;

    /* Initialisation logique et deck */
    init_game_logic(&g);
    init_uno_deck(&g.draw_pile);
    shuffle_deck(&g.draw_pile);
    init_player(&g.players[0], 0, "BotTest");

    /* Préparation main et talon */
    for(int i = 0; i < 7; i++) draw_to_hand(&g.players[0], &g.draw_pile);
    
    Card top = pop_card(&g.draw_pile);
    push_card(&g.discard_pile, top);
    g.active_color = -1; // Initialize correctly

    printf("=== ETAT DU JEU ===\nTalon : "); print_card_debug(top);
    printf("\nCouleur demandee : %d\n\n", g.active_color);

    /* Vérification de la main */
    printf("Analyse de la main :\n");
    for (int i = 0; i < g.players[0].hand.size; i++) {
        Card c = g.players[0].hand.cards[i];
        printf("%d. ", i + 1);
        print_card_debug(c);
        printf(is_move_valid(g.active_color, c, top) ? " -> [ ✅ ]\n" : " -> [ ❌ ]\n");
    }

    /* Nettoyage */
    clear_deck(&g.players[0].hand);
    clear_deck(&g.draw_pile);
    clear_deck(&g.discard_pile);
    printf("\nTest fini, memoire libre (structures statiques).\n");

    return 0;
}
