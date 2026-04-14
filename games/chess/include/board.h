/**
    @file board.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Board position utilities for chess.
*/
#ifndef BOARD_H
#define BOARD_H

#include "types.h"

/**
    @brief Get the board position from mouse coordinates.
    @return IVec2_st containing the board coordinates (column, row)
*/
IVec2_st getBoardPosition(void);

#endif
