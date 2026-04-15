/**
    @file game.h
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-04-14
    @brief Shared gameplay logic for bingo validation and win condition checks.
*/
#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

/**
    @brief Checks if a daub (mark) on a specific square is valid given the current call.
    
    A daub is valid if the number at the specified row and column matches the 
    currently called number, or if it's the free space (center).

    @param[in]     state        The current call state containing the called number.
    @param[in]     playerCard   The player's card containing the numbers.
    @param[in]     row          The row index (0-4).
    @param[in]     col          The column index (0-4).
    @return                     True if the daub is valid, false otherwise.
*/
bool bingo_isValidDaub(const CallState_St* const state, const PlayerCard_St* playerCard, uint row, uint col);

/**
    @brief Checks if the player has achieved a Bingo (5 in a row, column, or diagonal).

    @param[in]     playerCard   The player's card to check.
    @return                     True if a Bingo is found, false otherwise.
*/
bool bingo_hasBingo(const PlayerCard_St* const playerCard);

#endif // CORE_GAME_GAME_H