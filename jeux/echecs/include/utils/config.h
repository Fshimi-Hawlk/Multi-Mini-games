/**
 * @file config.h
 * @author Maxime CHAUVEAU
 * @brief Configuration constants for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file contains all the configuration constants used throughout
 * the game, including window dimensions, board size, and game limits.
 */

#ifndef UTILS_CONFIG_H
#define UTILS_CONFIG_H

/** @brief Window title */
#define WINDOW_NAME "Chess"

/** @brief Window width in pixels */
#define WINDOW_WIDTH 800

/** @brief Window height in pixels */
#define WINDOW_HEIGHT 600

/** @brief Number of pieces per player */
#define PIECES_PER_PLAYER 16

/** @brief Board size (8x8 for standard chess) */
#define BOARD_SIZE 8

/** @brief Size of each cell in pixels */
#define CELL_PX_SIZE (WINDOW_HEIGHT / (BOARD_SIZE + 2))

/** @brief Total board size in pixels */
#define BOARD_PX_SIZE (BOARD_SIZE * CELL_PX_SIZE)

/** @brief Board offset from window edge in pixels */
#define BOARD_OFFSET CELL_PX_SIZE

/** @brief Maximum number of moves that can be recorded */
#define NB_MAX_MOVE 250

#define SOUNDS_PATH "assets/sounds/"

#endif
