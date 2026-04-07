#include "core/game.h"
#include "ui/render.h"
#include "utils/globals.h"
#include "utils/utils.h"
#include "audio.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PUISSANCE4_WINDOW_WIDTH 1000
#define PUISSANCE4_WINDOW_HEIGHT 800
#define PUISSANCE4_CELL_SIZE 80
#define PUISSANCE4_PIECE_RADIUS 30

struct Puissance4Game_St {
    GameConfig_St config;
    VideoConfig_St videoStorage;
    AudioConfig_St audioStorage;
    Puissance4State_Et state;
    int board[PUISSANCE4_BOARD_ROWS][PUISSANCE4_BOARD_COLS];
    Puissance4Player_Et currentPlayer;
    Puissance4Player_Et winner;
    int selectedColumn;
    bool isRunning;
};

static void initBoard(Puissance4Game_St* game) {
    for (int row = 0; row < PUISSANCE4_BOARD_ROWS; row++) {
        for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
            game->board[row][col] = PUISSANCE4_PLAYER_NONE;
        }
    }
}

static bool isValidColumn(const Puissance4Game_St* game, int col) {
    if (col < 0 || col >= PUISSANCE4_BOARD_COLS) {
        return false;
    }
    return game->board[0][col] == PUISSANCE4_PLAYER_NONE;
}

static int dropPiece(Puissance4Game_St* game, int col) {
    if (!isValidColumn(game, col)) {
        return -1;
    }
    
    for (int row = PUISSANCE4_BOARD_ROWS - 1; row >= 0; row--) {
        if (game->board[row][col] == PUISSANCE4_PLAYER_NONE) {
            game->board[row][col] = game->currentPlayer;
            return row;
        }
    }
    return -1;
}

static bool checkWin(const Puissance4Game_St* game, int row, int col) {
    int player = game->board[row][col];
    if (player == PUISSANCE4_PLAYER_NONE) {
        return false;
    }
    
    int count = 0;
    for (int c = 0; c < PUISSANCE4_BOARD_COLS; c++) {
        if (game->board[row][c] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    count = 0;
    for (int r = 0; r < PUISSANCE4_BOARD_ROWS; r++) {
        if (game->board[r][col] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    int startRow = row - col;
    int startCol = 0;
    if (startRow > 0) {
        startCol = -startRow;
        startRow = 0;
    }
    count = 0;
    for (int i = 0; i < PUISSANCE4_BOARD_ROWS && startRow + i < PUISSANCE4_BOARD_ROWS && startCol + i < PUISSANCE4_BOARD_COLS; i++) {
        if (game->board[startRow + i][startCol + i] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    startRow = row + col;
    startCol = PUISSANCE4_BOARD_COLS - 1;
    if (startRow >= PUISSANCE4_BOARD_ROWS) {
        startCol = startRow - (PUISSANCE4_BOARD_ROWS - 1);
        startRow = PUISSANCE4_BOARD_ROWS - 1;
    }
    count = 0;
    for (int i = 0; i < PUISSANCE4_BOARD_ROWS && startRow - i >= 0 && startCol + i < PUISSANCE4_BOARD_COLS; i++) {
        if (game->board[startRow - i][startCol + i] == player) {
            count++;
            if (count >= 4) return true;
        } else {
            count = 0;
        }
    }
    
    return false;
}

static bool isBoardFull(const Puissance4Game_St* game) {
    for (int col = 0; col < PUISSANCE4_BOARD_COLS; col++) {
        if (game->board[0][col] == PUISSANCE4_PLAYER_NONE) {
            return false;
        }
    }
    return true;
}

static void switchPlayer(Puissance4Game_St* game) {
    game->currentPlayer = (game->currentPlayer == PUISSANCE4_PLAYER_1) ? 
                         PUISSANCE4_PLAYER_2 : PUISSANCE4_PLAYER_1;
}

static void resetGame(Puissance4Game_St* game) {
    initBoard(game);
    game->currentPlayer = PUISSANCE4_PLAYER_1;
    game->winner = PUISSANCE4_PLAYER_NONE;
    game->state = PUISSANCE4_STATE_PLAYING;
    game->selectedColumn = -1;
}

static void handleInput(Puissance4Game_St* game) {
    if (game->state != PUISSANCE4_STATE_PLAYING) {
        return;
    }
    
    int mouseX = GetMouseX();
    int boardWidth = PUISSANCE4_BOARD_COLS * PUISSANCE4_CELL_SIZE;
    int boardX = (PUISSANCE4_WINDOW_WIDTH - boardWidth) / 2;
    
    if (mouseX >= boardX && mouseX < boardX + boardWidth) {
        game->selectedColumn = (mouseX - boardX) / PUISSANCE4_CELL_SIZE;
    } else {
        game->selectedColumn = -1;
    }
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && game->selectedColumn >= 0) {
        int row = dropPiece(game, game->selectedColumn);
        if (row >= 0) {
            if (checkWin(game, row, game->selectedColumn)) {
                game->winner = game->currentPlayer;
                game->state = PUISSANCE4_STATE_GAME_OVER;
            } else if (isBoardFull(game)) {
                game->winner = PUISSANCE4_PLAYER_NONE;
                game->state = PUISSANCE4_STATE_GAME_OVER;
            } else {
                switchPlayer(game);
            }
        }
    }
}

Puissance4Game_St* puissance4Core_initGame(const GameConfig_St* config) {
    Puissance4Game_St* game = (Puissance4Game_St*)malloc(sizeof(Puissance4Game_St));
    if (!game) {
        return NULL;
    }
    
    if (config) {
        memcpy(&game->config, config, sizeof(GameConfig_St));
        if (game->config.video) {
            game->videoStorage = *game->config.video;
            game->config.video = &game->videoStorage;
        }
        if (game->config.audio) {
            game->audioStorage = *game->config.audio;
            game->config.audio = &game->audioStorage;
        }
    } else {
        game->config.audio = NULL;
        game->config.video = NULL;
    }
    
    game->state = PUISSANCE4_STATE_PLAYING;
    game->isRunning = true;
    
    initBoard(game);
    game->currentPlayer = PUISSANCE4_PLAYER_1;
    game->winner = PUISSANCE4_PLAYER_NONE;
    game->selectedColumn = -1;
    
    return game;
}

void puissance4Core_gameLoop(Puissance4Game_St* const game) {
    if (!game || !game->isRunning) {
        return;
    }
    
    handleInput(game);
    
    if (game->state == PUISSANCE4_STATE_GAME_OVER && IsKeyPressed(KEY_SPACE)) {
        resetGame(game);
    }
    
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->isRunning = false;
    }
    
    BeginDrawing();
    ClearBackground((Color){40, 40, 60, 255});
    
    puissance4UI_draw(game);
    
    EndDrawing();
}

void puissance4Core_freeGame(Puissance4Game_St* game) {
    if (game) {
        free(game);
    }
}

bool puissance4Core_isRunning(const Puissance4Game_St* game) {
    return game && game->isRunning;
}

int puissance4Core_getBoard(const Puissance4Game_St* game, int row, int col) {
    if (!game || row < 0 || row >= PUISSANCE4_BOARD_ROWS || col < 0 || col >= PUISSANCE4_BOARD_COLS) {
        return PUISSANCE4_PLAYER_NONE;
    }
    return game->board[row][col];
}

Puissance4State_Et puissance4Core_getState(const Puissance4Game_St* game) {
    return game ? game->state : PUISSANCE4_STATE_PLAYING;
}

Puissance4Player_Et puissance4Core_getCurrentPlayer(const Puissance4Game_St* game) {
    return game ? game->currentPlayer : PUISSANCE4_PLAYER_NONE;
}

Puissance4Player_Et puissance4Core_getWinner(const Puissance4Game_St* game) {
    return game ? game->winner : PUISSANCE4_PLAYER_NONE;
}

int puissance4Core_getSelectedColumn(const Puissance4Game_St* game) {
    return game ? game->selectedColumn : -1;
}