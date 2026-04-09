/**
    @file core/game.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-16
    @brief Core game logic implementation for Bingo.
*/

#include "core/game.h"
#include "utils/globals.h"
#include <string.h>

bool bingo_isValidDaub(const CallState_St* const state, const PlayerCard_St* playerCard, uint row, uint col) {
    if (row >= 5 || col >= 5) return false;
    return (state->column == col) 
        && (state->number == playerCard->numbers[row][col]);
}

bool bingo_hasBingo(const PlayerCard_St* const playerCard) {
    bool validMarked[5][5] = {0};
    for (u8 r = 0; r < 5; ++r) {
        for (u8 c = 0; c < 5; ++c) {
            validMarked[r][c] = playerCard->daubs[r][c] 
                             && (playerCard->misclicks[r][c] < 3);
        }
    }

    for (u8 r = 0; r < 5; ++r) {
        bool full = true;
        for (u8 c = 0; c < 5; ++c) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    for (u8 c = 0; c < 5; ++c) {
        bool full = true;
        for (u8 r = 0; r < 5; ++r) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    bool d1 = true, d2 = true;
    for (u8 i = 0; i < 5; ++i) {
        if (!validMarked[i][i])     d1 = false;
        if (!validMarked[i][4 - i]) d2 = false;
    }
    return d1 || d2;
}
