/**
    @file game.h
    @author Fshimi-Hawlk
    @date 2026-03-03
    @date 2026-04-14
    @brief Drawing functions for bingo cards and game-specific UI components.
*/
#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/userTypes.h"

/**
    @brief Draws the selection screen where players choose one of the available bingo cards.

    @param[in]     layout       Visual/layout positions and the choice cards data.
*/
void bingo_drawChoiceCards(const Layout_St* layout);

/**
    @brief Draws a specific player's bingo card, including numbers and daubed state.

    @param[in]     layout       Visual/layout positions for the card.
    @param[in]     player       The player's card data (numbers, daubs, misclicks).
*/
void bingo_drawCard(const Layout_St* const layout, const PlayerCard_St* const player);

#endif // UI_GAME_H