/**
    @file game.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief UI functions for general game information and previews.
*/

#ifndef UI_GAME_H
#define UI_GAME_H

#include "utils/types.h"

/**
    @brief Draws the game's UI information (score, level, etc.).
    
    Renders text display for score, current level, total lines cleared, and high score.

    @param[in] score           The current game score.
    @param[in] level           The current game level.
    @param[in] lineNbTotal     The total number of lines cleared.
    @param[in] highScore       The current session's high score.
*/
void tetrominoFall_drawInformations(int score, int level, int lineNbTotal, int highScore);

/**
    @brief Draws a preview of the current shape's landing position (ghost piece).
    
    Shows where the current tetromino would land if it were dropped instantly.

    @param[in] board           The current board state.
    @param[in] boardShape      The current falling shape state.
*/
void tetrominoFall_drawPreview(Board_t board, BoardShape_St boardShape);

#endif