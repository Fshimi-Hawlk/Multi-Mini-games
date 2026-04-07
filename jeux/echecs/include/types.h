/**
 * @file types.h
 * @author Maxime CHAUVEAU
 * @brief Type definitions for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file contains all the type definitions used throughout the game,
 * including piece types, player structures, and board representation.
 */

#ifndef TYPES_H
#define TYPES_H

#include "common.h"
#include "config.h"

/**
 * @brief Enumeration of chess piece names.
 */
typedef enum {
    PIECE_NAME_NONE,    /**< No piece */
    PIECE_NAME_PAWN,    /**< Pawn */
    PIECE_NAME_PONEY,   /**< Knight (called "Poney" in French) */
    PIECE_NAME_ROOK,    /**< Rook */
    PIECE_NAME_BISHOP,  /**< Bishop */
    PIECE_NAME_QUEEN,   /**< Queen */
    PIECE_NAME_KING     /**< King */
} PieceName_et;

/**
 * @brief Enumeration of piece order on the board.
 *
 * Defines the initial ordering of pieces from left to right
 * on the back rank and the pawn line.
 */
typedef enum {
    PIECE_ORDER_ROOK_1,     /**< First rook */
    PIECE_ORDER_PONEY_1,    /**< First knight */
    PIECE_ORDER_BISHOP_1,  /**< First bishop */
    PIECE_ORDER_QUEEN,     /**< Queen */
    PIECE_ORDER_KING,      /**< King */
    PIECE_ORDER_BISHOP_2,  /**< Second bishop */
    PIECE_ORDER_PONEY_2,  /**< Second knight */
    PIECE_ORDER_ROOK_2,    /**< Second rook */

    PIECE_ORDER_PAWN_1,    /**< First pawn */
    PIECE_ORDER_PAWN_2,    /**< Second pawn */
    PIECE_ORDER_PAWN_3,    /**< Third pawn */
    PIECE_ORDER_PAWN_4,    /**< Fourth pawn */
    PIECE_ORDER_PAWN_5,    /**< Fifth pawn */
    PIECE_ORDER_PAWN_6,    /**< Sixth pawn */
    PIECE_ORDER_PAWN_7,    /**< Seventh pawn */
    PIECE_ORDER_PAWN_8     /**< Eighth pawn */
} PieceOrder_et;

/**
 * @brief Enumeration of piece colors.
 */
typedef enum {
    COLOR_PIECE_NONE = -1,  /**< No color */
    COLOR_PIECE_WHITE,      /**< White pieces */
    COLOR_PIECE_BLACK      /**< Black pieces */
} ColorPiece_et;

/**
 * @brief 2D integer vector structure.
 *
 * Used to represent positions on the chess board.
 */
typedef struct {
    int x;  /**< X coordinate (column, 0-7) */
    int y;  /**< Y coordinate (row, 0-7) */
} IVec2_st;

/**
 * @brief Chess piece structure.
 *
 * Represents a single piece on the board with its properties.
 */
typedef struct {
    PieceName_et name;      /**< Type of piece */
    ColorPiece_et color;    /**< Color of piece (white or black) */
    IVec2_st pos;          /**< Current position on the board */
    bool isTaken;           /**< Whether the piece has been captured */
    bool canRock;           /**< Whether the piece can still castle */
} Piece_st;

/**
 * @brief Player structure.
 *
 * Contains all pieces belonging to a player.
 */
typedef struct {
    Piece_st* pieces[PIECES_PER_PLAYER];  /**< Array of player's pieces */
    ColorPiece_et color;                   /**< Player's color */
} Player_st;

/**
 * @brief Board type definition.
 *
 * 8x8 array of pointers to pieces representing the chess board.
 * A NULL value indicates an empty square.
 */
typedef Piece_st* Board_t[BOARD_SIZE][BOARD_SIZE];

#endif