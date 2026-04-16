/**
    @file utils.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Utility functions for chess.
*/
#include "utils.h"
#include "globals.h"

/**
    @brief Print all moves made in the game to stdout.
*/
void printMovesMade(void) {
    for (int i = 0; i < nbMoves; i++) {
        printf("%s ", movesPlayed[i]);
    }
    putchar('\n');
}

/**
    @brief Get the current mouse position as integer coordinates.
    @return IVec2_st containing mouse position
*/
IVec2_st GetMousePositionI(void) {
    Vector2 m = GetMousePosition();
    return (IVec2_st){(int)m.x, (int)m.y};
}