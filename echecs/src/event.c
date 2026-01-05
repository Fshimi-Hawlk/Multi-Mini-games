#include "event.h"
#include "global.h"
#include "algo.h"
#include "utils.h"
#include "board.h"

void handleEvents(Board_t board) {
    IVec2_st mousePos;

    if (WindowShouldClose()) {
        running = false;
        return;
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (finished || moveSimulationRendering) {
            return;
        }
        
        if (waitingForPromotion) {
            promotionChoice(board);
            return;
        }
        
        mousePos = getBoardPosition();

        if (isOOB(mousePos.y, mousePos.x)) {
            return;
        }

        if (board[mousePos.y][mousePos.x] && (board[mousePos.y][mousePos.x]->color == playerTurn)) {
            selectPiece(board, mousePos);
        }
        else if (selectionnedPiece) {
            if (!movement(board, selectionnedPiece, mousePos)) {
                return;
            }
            selectionnedPiece = NULL;
            playerTurn = !playerTurn;
        }
    }
}