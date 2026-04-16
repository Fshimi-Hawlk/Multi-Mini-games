/**
    @file config.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Configuration constants for the chess game.
*/
#ifndef CONFIG_H
#define CONFIG_H

/**
    @brief Window title
*/
#define WINDOW_NAME "Chess"

/**
    @brief Window width in pixels
*/
#define WINDOW_WIDTH 800

/**
    @brief Window height in pixels
*/
#define WINDOW_HEIGHT 600

/**
    @brief Number of pieces per player
*/
#define PIECES_PER_PLAYER 16

/**
    @brief Board size (8x8 for standard chess)
*/
#define BOARD_SIZE 8

/**
    @brief Size of each cell in pixels
*/
#define CELL_PX_SIZE (WINDOW_HEIGHT / (BOARD_SIZE + 2))

/**
    @brief Total board size in pixels
*/
#define BOARD_PX_SIZE (BOARD_SIZE * CELL_PX_SIZE)

/**
    @brief Board offset from window edge in pixels
*/
#define BOARD_OFFSET CELL_PX_SIZE

/**
    @brief Maximum number of moves that can be recorded
*/
#define NB_MAX_MOVE 250

/**
    @brief Path to the chess sound assets
*/
#define SOUNDS_PATH "chess/assets/sounds/"

#endif
