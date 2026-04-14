/**
    @file game.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Game logic and movement functions for Tetris.
*/
#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/types.h"
#include "utils/globals.h"

/**
    @brief Moves a shape automatically towards a target position and rotation.

    Used by the AI to perform moves.

    @param[in,out] speed        The current game speed/timing state.
    @param[in,out] boardShape   The shape to move.
    @param[in]     targetMove   The target position and rotation.
*/
void automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove);

/**
    @brief Handles manual player movement based on input.

    @param[in]     board       The current board state for collision checks.
    @param[in,out] boardShape  The shape to move.
*/
void mouvement(board_t board, boardShape_st* boardShape);

/**
    @brief Reads the high score from a file.

    @param[out] highScore  Pointer to store the high score.
*/
void readHighScore(int *highScore);

/**
    @brief Writes the score to the high score file if it's a new record.

    @param[in] highScore  Current high score.
    @param[in] score      Current game score.
*/
void writeHighScore(int highScore, int score);

#endif // CORE_GAME_H
