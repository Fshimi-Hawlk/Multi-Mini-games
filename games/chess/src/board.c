/**
    @file board.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Board position utilities for Chess.
*/
#include "board.h"
#include "global.h"

/**
    @brief Get the board position from mouse coordinates.
    @return IVec2_st containing the board coordinates (column, row)
*/
IVec2_st getBoardPosition(void) {
    Vector2 mouse = GetMousePosition();

    return (IVec2_st) {(mouse.x - BOARD_OFFSET) / CELL_PX_SIZE, (mouse.y - BOARD_OFFSET) / CELL_PX_SIZE};
}
