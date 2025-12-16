#include "core/animation/animation.h"
#include "core/game/boardUtils.h"
#include "core/game/board.h"
#include "core/game/botAlgo.h"
#include "utils/globals.h"
#include "utils/config.h"
#include "utils/types.h"

void initBoard(Board_t board) {
    // Standard starting position
    board[3][3] = board[4][4] = PLAYER;
    board[3][4] = board[4][3] = COMPUTER;
}

void flipInDirection(Board_t board, s64Vector2_St pos, s64Vector2_St dir, bool playerTurn) {
    CellState_Et piece = playerTurn ? PLAYER : COMPUTER;
    CellState_Et opponent = playerTurn ? COMPUTER : PLAYER;

    s64_t x = pos.x + dir.x;
    s64_t y = pos.y + dir.y;

    while (!isOutOfBounds((s64Vector2_St){x, y}) && board[y][x] == opponent) {
        board[y][x] = piece;
        x += dir.x;
        y += dir.y;
    }
}

void placePieceAndFlip(Board_t board, s64Vector2_St pos, bool playerTurn) {
    CellState_Et piece = playerTurn ? PLAYER : COMPUTER;
    board[pos.y][pos.x] = piece;

    for (s64_t dy = -1; dy <= 1; ++dy)
        for (s64_t dx = -1; dx <= 1; ++dx)
            if (!(dx == 0 && dy == 0)) {
                s64Vector2_St dir = {dx, dy};
                if (isDirectionValid(board, pos, dir, playerTurn))
                    flipInDirection(board, pos, dir, playerTurn);
            }
}

bool processTurn(
    Board_t board,
    s64Vector2_St clickedPos,
    s64Vector2_St* lastMove,
    s64Vector2_St flipped[], 
    u64_t* flippedCount
) {
    if (gameEnded) return true;

    if (isPlayerTurn) {
        playerHasMoves = hasValidMoves(board, true);

        if (!playerHasMoves) {
            isPlayerTurn = false;           // pass turn to computer
        } else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (isValidMove(board, clickedPos, true)) {
                *lastMove = clickedPos;
                collectFlippedPieces(board, clickedPos, true, flipped, flippedCount);
                placePieceAndFlip(board, clickedPos, true);
                isPlayerTurn = false;

                initAnimation(board, *lastMove, PLAYER, flipped, *flippedCount);
                return false;   // continue game
            }
        } else {
            return false;       // still waiting for player
        }
    } else if (!animationsPlaying) { // Computer turn
        computerHasMoves = hasValidMoves(board, false);

        if (computerHasMoves) {
            ValidMove_St candidates[BOARD_SIZE * BOARD_SIZE];
            u64_t count = 0;

            for (int y = 0; y < BOARD_SIZE; ++y)
                for (int x = 0; x < BOARD_SIZE; ++x) {
                    s64Vector2_St p = {x, y};
                    if (isValidMove(board, p, false)) {
                        candidates[count].pos   = p;
                        candidates[count].score = computeMoveScore(board, p, false);
                        count++;
                    }
                }

            if (count > 0) {
                u64_t best = findBestMoveIndex(candidates, count);
                *lastMove = candidates[best].pos;
                collectFlippedPieces(board, *lastMove, false, flipped, flippedCount);
                placePieceAndFlip(board, *lastMove, false);

                initAnimation(board, *lastMove, COMPUTER, flipped, *flippedCount);
            }
        }

        isPlayerTurn = true;
    }

    // Check end of game
    if (isBoardFull(board) || (!playerHasMoves && !computerHasMoves)) {
        countPieces(board, &playerScore, &computerScore);
        return true;
    }

    return false;
}