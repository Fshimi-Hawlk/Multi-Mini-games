/**
 * @file board.h
 * @author Maxime CHAUVEAU
 * @brief Board position utilities for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains functions for converting mouse positions
 * to board coordinates.
 */

#ifndef BOARD_H
#define BOARD_H

#include "types.h"

/**
 * @brief Get the board position from mouse coordinates.
 * @return IVec2_st containing the board coordinates (column, row)
 */
IVec2_st getBoardPosition(void);

#endif