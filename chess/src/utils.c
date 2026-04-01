/**
 * @file utils.c
 * @author Maxime CHAUVEAU
 * @brief Utility functions for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains utility functions for the chess game.
 */

#include "utils.h"
#include "global.h"

/**
 * @brief Print all moves made in the game to stdout.
 */
void printMovesMade(void) {
    for (int i = 0; i < nbMoves; i++) {
        printf("%s ", movesPlayed[i]);
    }
    putchar('\n');
}

/**
 * @brief Get the current mouse position as integer coordinates.
 * @return IVec2_st containing mouse position
 */
IVec2_st GetMousePositionI(void) {
    Vector2 m = GetMousePosition();
    return (IVec2_st){(int)m.x, (int)m.y};
}