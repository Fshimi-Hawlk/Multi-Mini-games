/**
 * @file bot.c
 * @author Gemini CLI
 * @date 2026-03-18
 * @brief Bot logic for King-for-Four (Uno).
 */

#include "core/bot.h"
#include "core/card.h"
#include <limits.h>

int evaluate_state(GameState* g, int player_id) {
    if (!g || player_id < 0 || player_id >= g->num_players) return -1000;

    int score = 0;
    Player* p = &g->players[player_id];
    
    // Penalize having cards in hand (points)
    Node* curr = p->hand.head;
    while(curr) {
        if (curr->card.value >= SKIP && curr->card.value <= PLUS_TWO) score -= 20;
        else if (curr->card.value >= JOKER) score -= 50;
        else score -= (int)curr->card.value;
        curr = curr->next;
    }

    // Reward having fewer cards than others
    for (int i = 0; i < g->num_players; i++) {
        if (i != player_id) {
            score += g->players[i].hand.size * 5;
        }
    }

    // Huge bonus if we have 0 cards
    if (p->hand.size == 0) score += 500;

    return score;
}

int calculate_best_move(GameState* g, int player_id, int* out_card_index) {
    if (!g || player_id < 0 || player_id >= g->num_players) return INT_MIN;

    int best_score = INT_MIN;
    int best_move = -1; // -1 means draw
    
    Card top_card = {CARD_BLACK, ZERO};
    if (g->discard_pile.head != NULL) {
        top_card = g->discard_pile.head->card;
    }

    Player* p = &g->players[player_id];
    Node* curr = p->hand.head;
    int idx = 0;
    while(curr) {
        if (is_move_valid(g->active_color, curr->card, top_card)) {
            // Try this move (hypothetically)
            // Since we don't have a full copy_game_state, we evaluate the card itself
            int move_score = 0;
            if (curr->card.value >= JOKER) move_score += 50;
            else if (curr->card.value >= SKIP) move_score += 30; // Prioritize actions
            else move_score += (int)curr->card.value;

            if (move_score > best_score) {
                best_score = move_score;
                best_move = idx;
            }
        }
        curr = curr->next;
        idx++;
    }

    *out_card_index = best_move;
    return best_score;
}
