#include "ui/render.h"
#include "core/game.h"
#include "raylib.h"

#define PUISSANCE4_WINDOW_WIDTH 1000
#define PUISSANCE4_WINDOW_HEIGHT 800
#define PUISSANCE4_CELL_SIZE 80
#define PUISSANCE4_PIECE_RADIUS 30

void puissance4UI_draw(const Puissance4Game_St* game) {
    puissance4UI_drawBoard(game);
    puissance4UI_drawUI(game);
}

void puissance4UI_drawBoard(const Puissance4Game_St* game) {
    int boardWidth = PUISSANCE4_BOARD_COLS * PUISSANCE4_CELL_SIZE;
    int boardHeight = PUISSANCE4_BOARD_ROWS * PUISSANCE4_CELL_SIZE;
    int boardX = (PUISSANCE4_WINDOW_WIDTH - boardWidth) / 2;
    int boardY = (PUISSANCE4_WINDOW_HEIGHT - boardHeight) / 2;
    
    DrawRectangle(boardX, boardY, boardWidth, boardHeight, (Color){0, 0, 180, 255});
    
    for (int row = 0; row < PUISSANCE4_BOARD_ROWS; row++) {
        for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
            int cellX = boardX + col * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
            int cellY = boardY + row * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
            
            Color pieceColor = WHITE;
            int cellValue = puissance4Core_getBoard(game, row, col);
            if (cellValue == PUISSANCE4_PLAYER_1) {
                pieceColor = (Color){255, 0, 0, 255};
            } else if (cellValue == PUISSANCE4_PLAYER_2) {
                pieceColor = (Color){255, 255, 0, 255};
            }
            
            DrawCircle(cellX, cellY, PUISSANCE4_PIECE_RADIUS, pieceColor);
        }
    }
    
    int selectedCol = puissance4Core_getSelectedColumn(game);
    if (selectedCol >= 0 && selectedCol < PUISSANCE4_BOARD_COLS) {
        int highlightX = boardX + selectedCol * PUISSANCE4_CELL_SIZE + PUISSANCE4_CELL_SIZE / 2;
        DrawRectangle(highlightX - PUISSANCE4_CELL_SIZE / 2, boardY - 10, PUISSANCE4_CELL_SIZE, 10, (Color){100, 100, 100, 150});
    }
}

void puissance4UI_drawUI(const Puissance4Game_St* game) {
    Puissance4Player_Et currentPlayer = puissance4Core_getCurrentPlayer(game);
    const char* playerText;
    Color playerColor;
    
    if (currentPlayer == PUISSANCE4_PLAYER_1) {
        playerText = "Player 1 (Red)";
        playerColor = (Color){255, 0, 0, 255};
    } else {
        playerText = "Player 2 (Yellow)";
        playerColor = (Color){255, 255, 0, 255};
    }
    
    DrawText(playerText, 20, 20, 24, playerColor);
    
    Puissance4State_Et state = puissance4Core_getState(game);
    if (state == PUISSANCE4_STATE_GAME_OVER) {
        Puissance4Player_Et winner = puissance4Core_getWinner(game);
        const char* winnerText;
        if (winner == PUISSANCE4_PLAYER_1) {
            winnerText = "Player 1 Wins!";
        } else if (winner == PUISSANCE4_PLAYER_2) {
            winnerText = "Player 2 Wins!";
        } else {
            winnerText = "It's a Draw!";
        }
        
        int textWidth = MeasureText(winnerText, 48);
        DrawText(winnerText, (PUISSANCE4_WINDOW_WIDTH - textWidth) / 2, PUISSANCE4_WINDOW_HEIGHT / 2 - 24, 48, WHITE);
        
        const char* restartText = "Press SPACE to restart";
        int restartWidth = MeasureText(restartText, 24);
        DrawText(restartText, (PUISSANCE4_WINDOW_WIDTH - restartWidth) / 2, PUISSANCE4_WINDOW_HEIGHT / 2 + 40, 24, LIGHTGRAY);
    }
    
    const char* instructions = "Click on column to drop piece | ESC to quit";
    int instrWidth = MeasureText(instructions, 16);
    DrawText(instructions, (PUISSANCE4_WINDOW_WIDTH - instrWidth) / 2, PUISSANCE4_WINDOW_HEIGHT - 30, 16, GRAY);
}