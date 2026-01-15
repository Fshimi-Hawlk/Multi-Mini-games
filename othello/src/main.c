#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BOARD_SIZE 8
#define CELL_SIZE (WINDOW_HEIGHT / (BOARD_SIZE + 2))

typedef int board_t[BOARD_SIZE][BOARD_SIZE];

enum {
    EMPTY, PLAYER, COMPUTER
};

typedef struct {
    int x, y;
} pos_st;

typedef struct {
    pos_st pos;
    int score;
} validCells_st;

Vector2 offset = {CELL_SIZE, CELL_SIZE};

void initBoard(board_t board);
bool isBoardFull(board_t board);
pos_st getBoardPosFromMousePos(Vector2 mousePos);
bool isOOB(pos_st boardPos);
bool checkDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn);
bool isBoardPosValid(board_t board, pos_st boardPos, bool isPlayerTurn);
bool canPlay(board_t board, bool isPlayerTurn);
bool isNextToCorner(pos_st boardPos);
int getScoreCell(board_t board, pos_st boardPos, bool isPlayerTurn);
int getIndexMaxScore(validCells_st validCells[BOARD_SIZE * BOARD_SIZE], int nbValidCells);
void updateBoard(board_t board, pos_st boardPos, bool isPlayerTurn);
void captureDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn);
void countTotalPieces(board_t board, int* playerTotal, int* computerTotal);
int countBoard(board_t board, pos_st boardPos, bool isPlayerTurn);
int countDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn);
void addFlippedCells(board_t board, pos_st boardPos, pos_st flippedCells[BOARD_SIZE * BOARD_SIZE], int* flippedCount, bool isPlayerTurn);

void DrawBoard(board_t board, pos_st flippedCells[BOARD_SIZE * BOARD_SIZE], int flippedCount, pos_st lastMove);

int main() {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Othello");
    SetTargetFPS(60);

    board_t board = {{EMPTY}};
    initBoard(board);

    bool isPlayerTurn = true;
    bool playerCanPlay = true, checkDown = false, computerCanPlay = true;
    int playerTotal = 0, computerTotal = 0;

    validCells_st validCells[BOARD_SIZE * BOARD_SIZE];
    int nbValidCells, indexChoice;
    
    pos_st boardPos;
    Vector2 mousePos;

    pos_st lastMove;
    pos_st flippedCells[BOARD_SIZE * BOARD_SIZE];
    int flippedCount = 0;

    bool finito = false;

    while (!WindowShouldClose()) {
        if (!finito && isPlayerTurn && (!checkDown || playerCanPlay)) {
            if (!checkDown) {
                playerCanPlay = canPlay(board, isPlayerTurn);
                checkDown = true;
            }
            if (!playerCanPlay) {
                isPlayerTurn = false;
                continue;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                mousePos = GetMousePosition();
                boardPos = getBoardPosFromMousePos(mousePos);
                if (isBoardPosValid(board, boardPos, isPlayerTurn)) {
                    updateBoard(board, boardPos, isPlayerTurn);
                    isPlayerTurn = false;
                    checkDown = false;
                }
            }
        }
        else if (!finito) {
            nbValidCells = 0;

            for (int j = 0; j < BOARD_SIZE; j++) {
                for (int i = 0; i < BOARD_SIZE; i++) {
                    boardPos = (pos_st) {.x = i, .y = j};
                    if (isBoardPosValid(board, boardPos, isPlayerTurn)) {
                        validCells[nbValidCells].pos = boardPos;
                        validCells[nbValidCells].score = getScoreCell(board, boardPos, isPlayerTurn);
                        nbValidCells++;
                    }
                }
            }

            if (nbValidCells) {
                indexChoice = getIndexMaxScore(validCells, nbValidCells);
                lastMove = validCells[indexChoice].pos;
                addFlippedCells(board, validCells[indexChoice].pos, flippedCells, &flippedCount, isPlayerTurn);
                updateBoard(board, validCells[indexChoice].pos, isPlayerTurn);
                computerCanPlay = true;
            }
            else
                computerCanPlay = false;
            isPlayerTurn = true;
        }
        
        if (!finito && ((!playerCanPlay && !computerCanPlay) || isBoardFull(board))) {
            countTotalPieces(board, &playerTotal, &computerTotal);
            finito = true;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawBoard(board, flippedCells, flippedCount, lastMove);
            if (finito) {
                DrawText(TextFormat("%s won !\n%d | %d", playerTotal > computerTotal ? "You" : "Computer", playerTotal > computerTotal ? playerTotal : computerTotal, playerTotal > computerTotal ? computerTotal : playerTotal), (BOARD_SIZE + 1) * CELL_SIZE + offset.x, WINDOW_HEIGHT / 3, 20, playerTotal > computerTotal ? BLUE : RED);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void initBoard(board_t board) {
    board[3][3] = board[4][4] = PLAYER;
    board[3][4] = board[4][3] = COMPUTER;
}

bool isBoardFull(board_t board) {
    for (int j = 0; j < BOARD_SIZE; j++)
        for (int i = 0; i < BOARD_SIZE; i++)
            if (board[j][i] == EMPTY)
                return false;
    return true;
}

pos_st getBoardPosFromMousePos(Vector2 mousePos) {
    if (mousePos.x < offset.x || mousePos.x > offset.x + BOARD_SIZE * CELL_SIZE || mousePos.y < offset.y || mousePos.y > offset.y + BOARD_SIZE * CELL_SIZE)
        return (pos_st) {.x = -1, .y = -1};

    return (pos_st) {.x = (mousePos.x - offset.x) / CELL_SIZE, .y = (mousePos.y - offset.y) / CELL_SIZE};
}

bool isOOB(pos_st boardPos) {
    return (boardPos.x < 0 || boardPos.x >= BOARD_SIZE || boardPos.y < 0 || boardPos.y >= BOARD_SIZE);
}

bool checkDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn) {
    bool foundOpponent = false;
    
    for (int i = boardPos.x + direction.x, j = boardPos.y + direction.y; !isOOB((pos_st) {.x = i, .y = j}); i += direction.x, j += direction.y) {
        if (board[j][i] == (isPlayerTurn ? COMPUTER : PLAYER))
            foundOpponent = true;
        else if (board[j][i] == (isPlayerTurn ? PLAYER : COMPUTER))
            return foundOpponent;
        else
            return false;
    }
    return false;
}

bool isBoardPosValid(board_t board, pos_st boardPos, bool isPlayerTurn) {
    if (isOOB(boardPos))
        return false;
    if (board[boardPos.y][boardPos.x] != EMPTY)
        return false;
    
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            if (i == 0 && j == 0) continue;
            if (checkDirection(board, boardPos, (pos_st) {.x = i, .y = j}, isPlayerTurn))
                return true;
        }
    }
    return false;
}

bool canPlay(board_t board, bool isPlayerTurn) {
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (isBoardPosValid(board, (pos_st) {.x = i, .y = j}, isPlayerTurn))
                return true;
        }
    }
    return false;
}

bool isNextToCorner(pos_st boardPos) {
    int x = boardPos.x;
    int y = boardPos.y;
    int last = BOARD_SIZE - 1;

    // Coin haut-gauche
    if (x <= 1 && y <= 1)
        return !(x == 0 && y == 0);

    // Coin haut-droit
    if (x >= last - 1 && y <= 1)
        return !(x == last && y == 0);

    // Coin bas-gauche
    if (x <= 1 && y >= last - 1)
        return !(x == 0 && y == last);

    // Coin bas-droit
    if (x >= last - 1 && y >= last - 1)
        return !(x == last && y == last);

    return false;
}

int getScoreCell(board_t board, pos_st boardPos, bool isPlayerTurn) {
    int score = 0;
    int x = boardPos.x, y = boardPos.y;

    if ((x == 0 || x == BOARD_SIZE - 1) && (y == 0 || y == BOARD_SIZE - 1))
        score += 100;

    else if (isNextToCorner(boardPos))
        score -= 50;

    else if (x == 0 || x == BOARD_SIZE - 1 || y == 0 || y == BOARD_SIZE - 1) {
        score += 50;
    }

    else if (x == 1 || x == BOARD_SIZE - 2 || y == 1 || y == BOARD_SIZE - 2) {
        score += -25;
    }
    score += 10 * countBoard(board, boardPos, isPlayerTurn);

    return score;
}

int getIndexMaxScore(validCells_st validCells[BOARD_SIZE * BOARD_SIZE], int nbValidCells) {
    int iMax = 0;

    for (int i = 1; i < nbValidCells; i++)
        if (validCells[i].score > validCells[iMax].score)
            iMax = i;

    return iMax;
}

void updateBoard(board_t board, pos_st boardPos, bool isPlayerTurn) {
    pos_st direction;
    board[boardPos.y][boardPos.x] = (isPlayerTurn ? PLAYER : COMPUTER);

    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            direction = (pos_st) {.x = i, .y = j};
            if (i == 0 && j == 0) continue;
            if (checkDirection(board, boardPos, direction, isPlayerTurn))
                captureDirection(board, boardPos, direction, isPlayerTurn);
        }
    }
}

void captureDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn) {
    for (int i = boardPos.x + direction.x, j = boardPos.y + direction.y; !isOOB((pos_st) {.x = i, .y = j}) && board[j][i] == (isPlayerTurn ? COMPUTER : PLAYER); i += direction.x, j += direction.y) {
        board[j][i] = (isPlayerTurn ? PLAYER : COMPUTER);
    }
}

void countTotalPieces(board_t board, int* playerTotal, int* computerTotal) {
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            if (board[j][i] == PLAYER)
                (*playerTotal)++;
            else if (board[j][i] == COMPUTER)
                (*computerTotal)++;
        }
    }
}

int countBoard(board_t board, pos_st boardPos, bool isPlayerTurn) {
    pos_st direction;
    int n = 0;
    
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            direction = (pos_st) {.x = i, .y = j};
            if (i == 0 && j == 0) continue;
            if (checkDirection(board, boardPos, direction, isPlayerTurn))
                n += countDirection(board, boardPos, direction, isPlayerTurn);
        }
    }
    return n;
}

int countDirection(board_t board, pos_st boardPos, pos_st direction, bool isPlayerTurn) {
    int n = 0;
    for (int i = boardPos.x + direction.x, j = boardPos.y + direction.y; !isOOB((pos_st) {.x = i, .y = j}) && board[j][i] == (isPlayerTurn ? COMPUTER : PLAYER); i += direction.x, j += direction.y) {
        n++;
    }
    return n;
}

void addFlippedCells(board_t board, pos_st boardPos, pos_st flippedCells[BOARD_SIZE * BOARD_SIZE], int* flippedCount, bool isPlayerTurn) {
    pos_st direction;
    *flippedCount = 0;
    
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            direction = (pos_st) {.x = i, .y = j};
            if (i == 0 && j == 0) continue;
            if (checkDirection(board, boardPos, direction, isPlayerTurn))
                for (int x = boardPos.x + direction.x, y = boardPos.y + direction.y; !isOOB((pos_st) {.x = x, .y = y}) && board[y][x] == (isPlayerTurn ? COMPUTER : PLAYER); x += direction.x, y += direction.y) {
                    flippedCells[(*flippedCount)++] = (pos_st) {.x = x, .y = y};
                }
        }
    }
}

void DrawBoard(board_t board, pos_st flippedCells[BOARD_SIZE * BOARD_SIZE], int flippedCount, pos_st lastMove) {
    for (int j = 0; j < BOARD_SIZE; j++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            DrawRectangle(i * CELL_SIZE + offset.x, j * CELL_SIZE + offset.y, CELL_SIZE, CELL_SIZE, GREEN);
            DrawRectangleLines(i * CELL_SIZE + offset.x, j * CELL_SIZE + offset.y, CELL_SIZE, CELL_SIZE, BLACK);

            if (board[j][i] == PLAYER) {
                DrawCircle(i * CELL_SIZE + offset.x + CELL_SIZE / 2, j * CELL_SIZE + offset.y + CELL_SIZE / 2, CELL_SIZE / 2 - 5, WHITE);
            } 
            else if (board[j][i] == COMPUTER) {
                DrawCircle(i * CELL_SIZE + offset.x + CELL_SIZE / 2, j * CELL_SIZE + offset.y + CELL_SIZE / 2, CELL_SIZE / 2 - 5, BLACK);
            }
        }
    }

    DrawCircle(lastMove.x * CELL_SIZE + CELL_SIZE / 2 + offset.x, lastMove.y * CELL_SIZE + CELL_SIZE / 2 + offset.y, CELL_SIZE / 2 - 2, RED);
    DrawCircle(lastMove.x * CELL_SIZE + CELL_SIZE / 2 + offset.x, lastMove.y * CELL_SIZE + CELL_SIZE / 2 + offset.y, CELL_SIZE / 2 - 5, BLACK);
    for (int i = 0; i < flippedCount; i++) {
        DrawCircle(flippedCells[i].x * CELL_SIZE + CELL_SIZE / 2 + offset.x, flippedCells[i].y * CELL_SIZE + CELL_SIZE / 2 + offset.y, CELL_SIZE / 2 - 2, ORANGE);
        DrawCircle(flippedCells[i].x * CELL_SIZE + CELL_SIZE / 2 + offset.x, flippedCells[i].y * CELL_SIZE + CELL_SIZE / 2 + offset.y, CELL_SIZE / 2 - 5, BLACK);

    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        DrawText(TextFormat("%d", i + 1), CELL_SIZE / 3, i * CELL_SIZE + offset.y + CELL_SIZE / 2 - 10, CELL_SIZE / 2, BLACK);
        DrawText(TextFormat("%d", i + 1), CELL_SIZE / 3 + BOARD_SIZE * CELL_SIZE + offset.x, i * CELL_SIZE + offset.y + CELL_SIZE / 2 - 10, CELL_SIZE / 2, BLACK);

        DrawText(TextFormat("%c", i + 'A'), i * CELL_SIZE + offset.x + CELL_SIZE / 2 - 10, CELL_SIZE / 3, CELL_SIZE / 2, BLACK);
        DrawText(TextFormat("%c", i + 'A'), i * CELL_SIZE + offset.x + CELL_SIZE / 2 - 10, CELL_SIZE / 3 + BOARD_SIZE * CELL_SIZE + offset.y, CELL_SIZE / 2, BLACK);
    }
}

