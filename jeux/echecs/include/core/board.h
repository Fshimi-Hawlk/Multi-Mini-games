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

#ifndef CORE_BOARD_H
#define CORE_BOARD_H

#include "utils/types.h"

/**
 * @brief Get the board position from mouse coordinates.
 * @return IVec2_st containing the board coordinates (column, row)
 */
IVec2_st getBoardPosition();

#endif
