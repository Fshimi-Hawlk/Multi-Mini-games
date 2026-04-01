/**
    @file core/game.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-19
    @brief One clear sentence that tells what this file is actually for.

    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "core/game.h"

#include "utils/globals.h"

bool bingo_isValidDaub(const CallState_St* const state, const PlayerCard_St* playerCard, uint row, uint col) {
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

    // Rows
    for (u8 r = 0; r < 5; ++r) {
        bool full = true;
        for (u8 c = 0; c < 5; ++c) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Columns (symmetric to rows)

    for (u8 c = 0; c < 5; ++c) {
        bool full = true;
        for (u8 r = 0; r < 5; ++r) {
            if (!validMarked[r][c]) { full = false; break; }
        }
        if (full) return true;
    }

    // Diagonals
    bool d1 = true, d2 = true;
    for (u8 i = 0; i < 5; ++i) {
        if (!validMarked[i][i])     d1 = false;
        if (!validMarked[i][4 - i]) d2 = false;
    }
    return d1 || d2;
}