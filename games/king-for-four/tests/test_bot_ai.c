/**
 * @file test_bot_ai.c
 * @brief Unit tests for Bot decision making in Uno.
 */

#include <stdio.h>
#include <assert.h>
#include "core/bot.h"
#include "core/card.h"

void test_bot_decision() {
    printf("Testing Bot AI Decisions...\n");
    GameState g;
    init_game_logic(&g);
    g.num_players = 2;
    init_player(&g.players[0], 0, "Bot");
    init_player(&g.players[1], 1, "Human");

    // Case 1: Bot has a playable card
    // Top card is Red 5
    Card top = {CARD_RED, FIVE};
    push_card(&g.discard_pile, top);
    g.active_color = -1; // No joker active

    // Bot hand: Blue 2, Red 9
    Card c1 = {CARD_BLUE, TWO};
    Card c2 = {CARD_RED, NINE};
    push_card(&g.players[0].hand, c1);
    push_card(&g.players[0].hand, c2);

    int move_idx = -1;
    calculate_best_move(&g, 0, &move_idx);

    // Bot should choose c2 (Red 9) because it matches color
    assert(move_idx == 1); // c2 was pushed last, so it's at index 1
    printf("  - Playable card selection: OK\n");

    // Case 2: Bot must draw
    clear_deck(&g.players[0].hand);
    Card c3 = {CARD_BLUE, TWO};
    push_card(&g.players[0].hand, c3); // Still Red 5 on top
    
    calculate_best_move(&g, 0, &move_idx);
    assert(move_idx == -1); // Must draw
    printf("  - Draw when no move: OK\n");
}

int main() {
    test_bot_decision();
    printf("ALL BOT AI TESTS PASSED!\n");
    return 0;
}
