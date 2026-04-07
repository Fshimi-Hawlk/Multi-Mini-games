/**
    @file ui/game.h
    @author Fshimi-Hawlk
    @date 2026-03-03
    @date 2026-03-05
    @brief One clear sentence that tells what this file is actually for.

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

void bingo_drawChoiceCards(const BingoGame_St* const game);

/**
    @brief Draws the player card.

    TODO
*/
void bingo_drawCard(const BingoGame_St* const game);

#endif // UI_GAME_H