#include "core/game.h"
#include "core/card.h"
#include <stddef.h>
#include <stdio.h>

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


// ... (tes tableaux valid_color / valid_value restent ici) ...

// ... (tes fonctions init et is_move_valid restent ici) ...

int try_play_card(GameState *g, int playerIndex, int cardIndex) {
    Player* p = &g->players[playerIndex];
    
    // 1. Trouver la carte dans la main sans la retirer 
    Node* current = p->hand.head;
    for (int i = 0; i < cardIndex; i++) {
        if (current) current = current->next;
    }
    if (!current) return 0; // Erreur d'index

    Card cardToPlay = current->card;
    
    // 2. Récupérer la carte du dessus du talon
    if (g->discard_pile.head == NULL) return 0; // Erreur technique
    Card topCard = g->discard_pile.head->card;

    // 3. Vérifier les règles
    if (is_move_valid(g->active_color, cardToPlay, topCard)) {
        
        // --- LE COUP EST VALIDE ---
        
        // A. On retire la carte de la main
        Card played = remove_at(&p->hand, cardIndex);
        
        // B. On la met sur le talon
        push_card(&g->discard_pile, played);
        
        // C. On met à jour la couleur active
        if (played.color == CARD_BLACK) {
            // TODO: Ouvrir un menu pour choisir la couleur
            g->active_color = CARD_RED; // Temporaire pour tester
        } else {
            g->active_color = played.color;
        }
        
        printf("Carte jouée ! Reste : %d cartes\n", p->hand.size);
        return 1; // Succès
    }
    
    printf("Coup invalide !\n");
    return 0; // Échec
}

int player_draw_card(GameState *g, int playerIndex) {
    Player* p = &g->players[playerIndex];

    // 1. Si la pioche est vide, on recycle le talon
    if (g->draw_pile.size == 0) {
        if (g->discard_pile.size <= 1) {
            printf("Plus aucune carte nulle part !\n");
            return 0; // Vraiment plus de cartes
        }

        printf("Pioche vide ! On mélange le talon...\n");
        
        // On garde la carte du dessus du talon
        Card topVisible = pop_card(&g->discard_pile);
        
        // On déplace tout le reste du talon vers la pioche
        while(g->discard_pile.size > 0) {
            push_card(&g->draw_pile, pop_card(&g->discard_pile));
        }
        
        // On remet la carte visible sur le talon
        push_card(&g->discard_pile, topVisible);
        
        // On mélange la nouvelle pioche
        shuffle_deck(&g->draw_pile);
    }

    // 2. On pioche
    draw_to_hand(p, &g->draw_pile);
    return 1;
}