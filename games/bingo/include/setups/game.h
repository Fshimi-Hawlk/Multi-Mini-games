/**
    @file game.h
    @author Kimi BERGE
    @date 2026-03-02
    @date 2026-04-14
    @brief Initialization of game state, layout, and card generation.
*/
#ifndef SETUPS_GAME_H
#define SETUPS_GAME_H

#include "utils/userTypes.h"

/**
    @brief Calculates and sets up the layout positions for the game UI elements.

    @param[out]    layout       The layout structure to be filled with computed positions.
*/
void bingo_computeLayout(Layout_St* layout);

/**
    @brief Generates a random 5x5 bingo card using the provided pool of available numbers.

    @param[out]    card         The card to be filled with generated numbers.
    @param[in,out] available    Pool of available numbers to pick from.
    @param[in]     count        Number of elements currently in the available pool.
    @return                     True if generation was successful, false otherwise.
*/
bool bingo_generateCard(Card_t card, uint *available, uint count);

#endif // SETUPS_GAME_H