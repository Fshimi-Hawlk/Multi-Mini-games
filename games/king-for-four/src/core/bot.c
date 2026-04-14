/**
    @file bot.c
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Bot logic for King-for-Four (Uno).
*/
#include "core/bot.h"
#include "core/card.h"
#include <limits.h>

/**
    @brief Scores used by the AI to evaluate moves and states.
*/
typedef enum {
    SCORE_BASIC_CARD  = 10,     ///< Base score for numerical cards
    SCORE_ACTION_CARD = 30,     ///< Score for action cards (Skip, Reverse, etc.)
    SCORE_WILD_CARD   = 50,     ///< Score for wild cards (Joker, Plus Four)
    SCORE_WIN_BONUS   = 500     ///< Bonus for winning the game
} AIScore_Et;

/**
    @brief Evaluates the game state for a specific player.

    @param[in]     g            Pointer to the game state.
    @param[in]     player_id    Index of the player.
    @return                     The score of the state (higher is better).
*/
int kingForFour_evaluateState(KingForFourGameState_St* g, int playerId) {
    if (!g || playerId < 0 || playerId >= g->numPlayers) return -1000;

    int score = 0;
    Player_St* p = &g->players[playerId];
    
    // Penalize having cards in hand (points)
    for (int i = 0; i < p->hand.size; i++) {
        Card_St c = p->hand.cards[i];
        if (c.value >= SKIP && c.value <= PLUS_TWO) score -= 20;
        else if (c.value >= JOKER) score -= (int)SCORE_WILD_CARD;
        else score -= (int)c.value;
    }

    // Reward having fewer cards than others
    for (int i = 0; i < g->numPlayers; i++) {
        if (i != playerId) {
            score += g->players[i].hand.size * 5;
        }
    }

    // Huge bonus if we have 0 cards
    if (p->hand.size == 0) score += (int)SCORE_WIN_BONUS;

    return score;
}

/**
    @brief Calculates the best move for a bot using a simplified heuristic.

    @param[in,out] g               Pointer to the game state.
    @param[in]     player_id       Index of the bot player.
    @param[out]    out_card_index  Pointer to store the index of the chosen card (-1 for draw).
    @return                        The score of the chosen move.
*/
int kingForFour_calculateBestMove(KingForFourGameState_St* g, int playerId, int* outCardIndex) {
    if (!g || playerId < 0 || playerId >= g->numPlayers || !outCardIndex) return INT_MIN;

    int bestScore = INT_MIN;
    int bestMove = -1; // -1 means draw
    
    Card_St topCard = {CARD_BLACK, ZERO};
    if (g->discardPile.size > 0) {
        topCard = g->discardPile.cards[g->discardPile.size - 1];
    }

    Player_St* p = &g->players[playerId];
    
    for (int i = 0; i < p->hand.size; i++) {
        Card_St c = p->hand.cards[i];
        if (kingForFour_isMoveValid(g->activeColor, c, topCard)) {
            // Try this move (hypothetically)
            // Since we don't have a full copy_gameState, we evaluate the card itself
            int moveScore = 0;
            if (c.value >= JOKER) moveScore += SCORE_WILD_CARD;
            else if (c.value >= SKIP) moveScore += SCORE_ACTION_CARD; // Prioritize actions
            else moveScore += (int)c.value;

            if (moveScore > bestScore) {
                bestScore = moveScore;
                bestMove = i;
            }
        }
    }

    *outCardIndex = bestMove;
    return bestScore;
}
