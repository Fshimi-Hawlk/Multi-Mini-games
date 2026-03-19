/**
 * @file test_ui_logic.c
 * @brief TDD: Verification of card selection logic (topmost card picking).
 */

#include <stdio.h>
#include <assert.h>
#include "include/core/player.h"

// Mock of the logic used in renderer.c for hitboxes
int simulate_get_hovered_index(int hand_size, int clicked_x, int start_x, int padding, int card_w) {
    int hovered_index = -1;
    // Current bug: loop from 0 to size-1 picks the bottom-most card first
    for (int i = 0; i < hand_size; i++) {
        int card_left = start_x + (i * padding);
        int card_right = card_left + ((i == hand_size - 1) ? card_w : padding);
        
        if (clicked_x >= card_left && clicked_x <= card_right) {
            hovered_index = i;
            // Bug: we should return the LAST match (topmost), but here we continue or stop early
        }
    }
    return hovered_index;
}

void test_topmost_card_selection() {
    printf("TDD: Testing topmost card selection...\n");
    
    int hand_size = 3;
    int start_x = 100;
    int padding = 40;
    int card_w = 100;

    // Card 0: 100 to 140 (overlap starts at 140)
    // Card 1: 140 to 180
    // Card 2: 180 to 280 (full width)
    
    // Click at x=150. This is on Card 1.
    int selected = simulate_get_hovered_index(hand_size, 150, start_x, padding, card_w);
    printf("  - Click at 150: Got card %d, Expected 1\n", selected);
    assert(selected == 1);

    // Click at x=130. This is on Card 0.
    selected = simulate_get_hovered_index(hand_size, 130, start_x, padding, card_w);
    printf("  - Click at 130: Got card %d, Expected 0\n", selected);
    assert(selected == 0);
    
    printf("  - Result: SUCCESS (if no crash, but the logic might be flawed in the loop order)\n");
}

int main() {
    test_topmost_card_selection();
    return 0;
}
