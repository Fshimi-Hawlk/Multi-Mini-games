#ifndef TYPES_H
#define TYPES_H

#include "common.h"
#include "config.h"

typedef enum {
    PIECE_NAME_NONE,
    PIECE_NAME_PAWN,
    PIECE_NAME_PONEY,
    PIECE_NAME_ROOK,
    PIECE_NAME_BISHOP,
    PIECE_NAME_QUEEN,
    PIECE_NAME_KING
} PieceName_et;

typedef enum {
    PIECE_ORDER_ROOK_1,
    PIECE_ORDER_PONEY_1,
    PIECE_ORDER_BISHOP_1,
    PIECE_ORDER_QUEEN,
    PIECE_ORDER_KING,
    PIECE_ORDER_BISHOP_2,
    PIECE_ORDER_PONEY_2,
    PIECE_ORDER_ROOK_2,

    PIECE_ORDER_PAWN_1,
    PIECE_ORDER_PAWN_2,
    PIECE_ORDER_PAWN_3,
    PIECE_ORDER_PAWN_4,
    PIECE_ORDER_PAWN_5,
    PIECE_ORDER_PAWN_6,
    PIECE_ORDER_PAWN_7,
    PIECE_ORDER_PAWN_8
} PieceOrder_et;

typedef enum {
    COLOR_PIECE_NONE = -1,
    COLOR_PIECE_WHITE,
    COLOR_PIECE_BLACK
} ColorPiece_et;

typedef struct {
    int x, y;
} IVec2_st;

typedef struct {
    PieceName_et name;
    ColorPiece_et color;
    IVec2_st pos;
    bool isTaken;
    bool canRock;
} Piece_st;

typedef struct {
    Piece_st* pieces[PIECES_PER_PLAYER];
    ColorPiece_et color;
} Player_st;

typedef Piece_st* Board_t[BOARD_SIZE][BOARD_SIZE];

#endif