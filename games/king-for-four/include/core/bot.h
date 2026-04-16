/**
    @file bot.h
    @author i-Charlys
    @date 2026-03-18
    @date 2026-04-14
    @brief Bot logic for King-for-Four (Uno).
*/
#ifndef BOT_H
#define BOT_H

#include "game.h"

/**
    @brief Evaluates the game state for a specific player.
    
    @param g Pointer to the KingForFourGameState_St.
    @param player_id Index of the player.
    @return The score of the state (higher is better).
*/
int kingForFour_evaluateState(KingForFourGameState_St* g, int player_id);

/**
    @brief Calculates the best move for a bot using a simplified Negamax/Minimax.

    @param[in,out] g               Pointer to the GameState_St.
    @param[in]     player_id       Index of the bot.
    @param[out]    out_card_index  Pointer to store the index of the card to play (-1 for draw).
    @return                        The evaluation score of the chosen move.
*/
int kingForFour_calculateBestMove(KingForFourGameState_St* g, int player_id, int* out_card_index);

#endif // BOT_H
