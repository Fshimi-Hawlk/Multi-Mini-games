#include "ai.h"
#include "algo.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define MATERIAL_PAWN 100
#define MATERIAL_KNIGHT 320
#define MATERIAL_BISHOP 330
#define MATERIAL_ROOK 500
#define MATERIAL_QUEEN 900
#define MATERIAL_KING 20000

// Positional bonuses (encouraging center control)
static const s32 pawnTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

static const s32 knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

static const s32 bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

static const s32 rookTable[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10, 10, 10, 10, 10,  5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    {-5,  0,  0,  0,  0,  0,  0, -5},
    { 0,  0,  0,  5,  5,  0,  0,  0}
};

static const s32 queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

static const s32 kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

static s32 getPieceValue(Piece_st* p, s32 x, s32 y) {
    if (!p) return 0;
    s32 val = 0;
    s32 tableX = (x < 0) ? 0 : (x > 7 ? 7 : x);
    s32 tableY = (p->color == COLOR_PIECE_WHITE) ? (7 - y) : y;
    tableY = (tableY < 0) ? 0 : (tableY > 7 ? 7 : tableY);

    switch (p->name) {
        case PIECE_NAME_PAWN:   val = MATERIAL_PAWN + pawnTable[tableY][tableX]; break;
        case PIECE_NAME_PONEY:  val = MATERIAL_KNIGHT + knightTable[tableY][tableX]; break;
        case PIECE_NAME_BISHOP: val = MATERIAL_BISHOP + bishopTable[tableY][tableX]; break;
        case PIECE_NAME_ROOK:   val = MATERIAL_ROOK + rookTable[tableY][tableX]; break;
        case PIECE_NAME_QUEEN:  val = MATERIAL_QUEEN + queenTable[tableY][tableX]; break;
        case PIECE_NAME_KING:   val = MATERIAL_KING + kingTable[tableY][tableX]; break;
        default: break;
    }
    return (p->color == COLOR_PIECE_WHITE) ? val : -val;
}

s32 ai_evaluateBoard(Board_t board) {
    s32 total = 0;
    for (s32 y = 0; y < 8; y++) {
        for (s32 x = 0; x < 8; x++) {
            total += getPieceValue(board[y][x], x, y);
        }
    }
    return total;
}

static Piece_st* simulateMove(Board_t board, IVec2_st from, IVec2_st to) {
    Piece_st* moving = board[from.y][from.x];
    Piece_st* captured = board[to.y][to.x];
    
    if (captured) captured->isTaken = true;
    
    board[to.y][to.x] = moving;
    board[from.y][from.x] = NULL;
    moving->pos = to;
    
    return captured;
}

static void undoMove(Board_t board, IVec2_st from, IVec2_st to, Piece_st* captured) {
    Piece_st* moving = board[to.y][to.x];
    
    board[from.y][from.x] = moving;
    board[to.y][to.x] = captured;
    moving->pos = from;
    if (captured) captured->isTaken = false;
}

static s32 minimax(Board_t board, Player_st* white, Player_st* black, u8 depth, s32 alpha, s32 beta, bool isMaximizing) {
    if (depth == 0) return ai_evaluateBoard(board);

    Player_st* currentPlayer = isMaximizing ? white : black;
    if (!currentPlayer) return 0;

    s32 bestVal = isMaximizing ? -2000000 : 2000000;

    for (s32 i = 0; i < PIECES_PER_PLAYER; i++) {
        Piece_st* p = currentPlayer->pieces[i];
        if (!p || p->isTaken) continue;

        IVec2_st from = p->pos;
        for (s32 ty = 0; ty < 8; ty++) {
            for (s32 tx = 0; tx < 8; tx++) {
                if (canBePlaced(board, p, tx, ty)) {
                    if (!isInCheck(board, p, tx, ty, isMaximizing ? 0 : 1)) {
                        IVec2_st to = {tx, ty};
                        Piece_st* captured = simulateMove(board, from, to);
                        if (!board[to.y][to.x]) { // Safety check if simulateMove failed
                             undoMove(board, from, to, captured);
                             continue;
                        }

                        s32 val = minimax(board, white, black, depth - 1, alpha, beta, !isMaximizing);

                        undoMove(board, from, to, captured);

                        if (isMaximizing) {
                            if (val > bestVal) bestVal = val;
                            if (bestVal > alpha) alpha = bestVal;
                        } else {
                            if (val < bestVal) bestVal = val;
                            if (bestVal < beta) beta = bestVal;
                        }
                        if (beta <= alpha) return bestVal;
                    }
                }
            }
        }
    }
    return bestVal;
}

ChessMove_st ai_getBestMove(Board_t board, Player_st* white, Player_st* black, s32 player, u8 depth) {
    ChessMove_st bestMove = {{-1, -1}, {-1, -1}, (player == 0) ? -2000000 : 2000000};
    bool isMaximizing = (player == 0);
    s32 alpha = -2000000;
    s32 beta = 2000000;

    // Hard limit depth to 2 to prevent stack overflow/crash during intensive simulation
    if (depth > 2) depth = 2;

    Player_st* currentPlayer = isMaximizing ? white : black;
    if (!currentPlayer) return bestMove;

    for (s32 i = 0; i < PIECES_PER_PLAYER; i++) {
        Piece_st* p = currentPlayer->pieces[i];
        if (!p || p->isTaken) continue;

        IVec2_st from = p->pos;
        for (s32 ty = 0; ty < 8; ty++) {
            for (s32 tx = 0; tx < 8; tx++) {
                if (canBePlaced(board, p, tx, ty)) {
                    if (!isInCheck(board, p, tx, ty, player)) {
                        IVec2_st to = {tx, ty};
                        Piece_st* captured = simulateMove(board, from, to);
                        if (!board[to.y][to.x]) {
                            undoMove(board, from, to, captured);
                            continue;
                        }

                        s32 val = minimax(board, white, black, depth - 1, alpha, beta, !isMaximizing);

                        undoMove(board, from, to, captured);

                        if (isMaximizing) {
                            if (val > bestMove.score) {
                                bestMove.score = val;
                                bestMove.from = from;
                                bestMove.to = to;
                            }
                        } else {
                            if (val < bestMove.score) {
                                bestMove.score = val;
                                bestMove.from = from;
                                bestMove.to = to;
                            }
                        }
                    }
                }
            }
        }
    }
    return bestMove;
}
