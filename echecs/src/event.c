#include "event.h"
#include "global.h"

void handleEvents(Board_t board) {
    Vector2 mousePos = GetMousePosition();

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

        if (isOOB(mousePos)) {
            return;
        }

        int mouseX = mousePos.x;
        int mouseY = mousePos.y;

        if (board[mouseY][mouseX] && (board[mouseY][mouseX]->color == playerTurn)) {
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