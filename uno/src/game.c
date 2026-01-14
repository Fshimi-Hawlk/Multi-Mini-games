#include "game.h"
#include <stddef.h>

/* Règle des couleurs : [Talon (0-3)][Joué (0-4)] */
int valid_color[4][5] = {
    {1, 0, 0, 0, 1},
    {0, 1, 0, 0, 1},
    {0, 0, 1, 0, 1},
    {0, 0, 0, 1, 1}
};

/* Règle des symboles : [Talon (0-14)][Joué (0-14)] */
int valid_value[15][15] = {0};

/**
 * Initialisation de la logique et de l'état
 */
void init_game_logic(GameState* g) {
    for (int i = 0; i < 15; i++) valid_value[i][i] = 1;

    if (!g) return;
    g->current_player = 0;
    g->game_direction = 1;
    g->active_color = -1;
    g->num_players = 0;
    g->draw_pile = (Deck){NULL, 0};
    g->discard_pile = (Deck){NULL, 0};
}

/**
 * Validation du coup (Retourne 1 si valide, 0 sinon)
 */
int is_move_valid(int active_color, Card played, Card top) {
    int idx = (active_color >= 0 && active_color < 4) ? active_color : top.color;
    
    int color_ok = (idx >= 0 && idx < 4) ? valid_color[idx][played.color] : 0;
    int value_ok = valid_value[top.value][played.value];

    return color_ok || value_ok;
}

/**
 * Distribution initiale et mise en place du talon
 */
void distribute_cards(GameState* g) {
    if (!g || g->num_players <= 0) return;

    for (int i = 0; i < g->num_players; i++) {
        for (int j = 0; j < 7; j++) draw_to_hand(&(g->players[i]), &(g->draw_pile));
    }

    Card first = pop_card(&(g->draw_pile));
    push_card(&(g->discard_pile), first);
    
    /* Couleur forcée si la première carte est un Joker */
    g->active_color = (first.color == 4) ? 0 : first.color;
}