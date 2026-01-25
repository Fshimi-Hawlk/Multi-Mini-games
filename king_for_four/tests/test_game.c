#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/core/game.h"

/* Debug : Affiche le nom d'une carte */
void print_card_debug(Card c) {
    const char *clrs[] = {"ROUGE", "JAUNE", "VERT", "BLEU", "NOIR"};
    const char *vals[] = {"0","1","2","3","4","5","6","7","8","9","SKIP","REVERSE","+2","JOKER","+4"};
    printf("[%s %s]", clrs[c.color], vals[c.value]);
}

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
    g.active_color = (top.color == 4) ? 0 : top.color;

    printf("=== ETAT DU JEU ===\nTalon : "); print_card_debug(top);
    printf("\nCouleur demandee : %d\n\n", g.active_color);

    /* Vérification de la main */
    printf("Analyse de la main :\n");
    Node *curr = g.players[0].hand.head;
    for (int i = 1; curr; i++, curr = curr->next) {
        printf("%d. ", i);
        print_card_debug(curr->card);
        printf(is_move_valid(g.active_color, curr->card, top) ? " -> [ ✅ ]\n" : " -> [ ❌ ]\n");
    }

    /* Nettoyage */
    free_deck(&g.players[0].hand);
    free_deck(&g.draw_pile);
    free_deck(&g.discard_pile);
    printf("\nTest fini, memoire libre.\n");

    return 0;
}