/**
 * @file event.c
 * @author Maxime CHAUVEAU
 * @brief Event handling for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains functions for handling user input events.
 */

#include "event.h"
#include "global.h"
#include "algo.h"
#include "utils.h"
#include "board.h"

/**
 * @brief Handle all input events for the current frame.
 * @param board The game board
 */
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