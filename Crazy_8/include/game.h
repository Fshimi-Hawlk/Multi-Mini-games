#ifndef GAME_H
#define GAME_H

#include "player.h"

extern int valid_color[4][5];
extern int valid_value[15][15];

// Structure pour gérer toute la partie
typedef struct {
    Player players[4];    // Tes joueurs
    int num_players;
    int current_player;   // Index (0, 1, 2 ou 3)
    int game_direction;   // 1 pour sens horaire, -1 pour anti-horaire
    int active_color;     // La couleur imposée (0-3)
    Deck draw_pile;       // La pioche
    Deck discard_pile;    // Le talon (le tas de défausse)
} GameState;

// Prototypes
void init_game_logic(GameState* g);
int is_move_valid(int active_color, Card played, Card top);
void distribute_cards(GameState* g);

#endif