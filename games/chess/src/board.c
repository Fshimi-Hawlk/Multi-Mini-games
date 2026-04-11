/**
 * @file board.c
 * @author Maxime CHAUVEAU
 * @brief Board position utilities for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains functions for converting mouse positions
 * to board coordinates.
 */

#include "board.h"
#include "global.h"

/**
 * @brief Get the board position from mouse coordinates.
 * @return IVec2_st containing the board coordinates (column, row)
 */
IVec2_st getBoardPosition(void) {
    Vector2 mouse = GetMousePosition();

    return (IVec2_st) {(mouse.x - BOARD_OFFSET) / CELL_PX_SIZE, (mouse.y - BOARD_OFFSET) / CELL_PX_SIZE};
}