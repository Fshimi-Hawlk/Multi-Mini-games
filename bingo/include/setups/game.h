/**
    @file game.h (setups)
    @author Fshimi Hawlk
    @date 2026-03-02
    @date 2026-03-19
    @brief Full game state initialization.
*/

#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/userTypes.h"

void bingo_computeLayout(Layout_St* layout);
bool bingo_generateCard(Card_t card, uint *available, uint count);

#endif // SETUPS_GAME_H