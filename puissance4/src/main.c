#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define WIDTH 7
#define HEIGHT 6
#define CELL_SIZE (WINDOW_HEIGHT / (HEIGHT + 2))

typedef int board_t[HEIGHT][WIDTH];

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

pos_st getBoardPosFromMousePos(Vector2 mousePos);
bool isOOB(pos_st boardPos);
bool isBoardPosValid(board_t board, pos_st boardPos);
void updateBoard(board_t board, pos_st boardPos, bool isPlayerTurn, pos_st* lastMove);
int checkAlignment(board_t board, int x, int y, int dx, int dy);
int whoWins(board_t board);

pos_st bestMove(board_t board, validCells_st* validCells, int validCellsCount);
int evaluateBoard(board_t board, pos_st pos);

void drawBoard(board_t board);

int main() {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Puissance 4");
    SetTargetFPS(60);

    board_t board = {{EMPTY}};

    pos_st boardPos;
    Vector2 mousePos;
    pos_st lastMove = {-1, -1};
    validCells_st validCells[WIDTH];
    int validCellsCount = 0;

    bool isPlayerTurn = true;
    int whoWon = 0;

    while (!WindowShouldClose()) {
        if (isPlayerTurn && !whoWon) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                mousePos = GetMousePosition();
                boardPos = getBoardPosFromMousePos(mousePos);
                if (isBoardPosValid(board, boardPos)) {
                    updateBoard(board, boardPos, isPlayerTurn, &lastMove);
                    isPlayerTurn = false;
                    whoWon = whoWins(board);
                }
            }
        }
        else if (!whoWon) {
            validCellsCount = 0;
            for (boardPos = (pos_st) {.x = 0, .y = 0}; boardPos.x < WIDTH; (boardPos.x)++) {
                if (isBoardPosValid(board, boardPos)) {
                    for (boardPos.y = HEIGHT - 1; board[boardPos.y][boardPos.x]; boardPos.y--);
                    validCells[validCellsCount++].pos = boardPos;
                }
            }
            boardPos = bestMove(board, validCells, validCellsCount);

            updateBoard(board, boardPos, isPlayerTurn, &lastMove);
            isPlayerTurn = true;
            whoWon = whoWins(board);
        }

        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            drawBoard(board);
            if (whoWon)
                DrawText(TextFormat("%s", whoWon == PLAYER ? "You won" : whoWon == COMPUTER ? "Computer won" : "It's a draw"), WIDTH * CELL_SIZE + offset.x + 10, WINDOW_HEIGHT / 3, 20, whoWon == PLAYER ? BLUE : whoWon == COMPUTER ? RED : GREEN);
            if (lastMove.x != -1 && lastMove.y != -1 && isPlayerTurn) {
                DrawCircle(lastMove.x * CELL_SIZE + offset.x + CELL_SIZE / 2, lastMove.y * CELL_SIZE + offset.y + CELL_SIZE / 2, CELL_SIZE / 2 - 2, BLACK);
                DrawCircle(lastMove.x * CELL_SIZE + offset.x + CELL_SIZE / 2, lastMove.y * CELL_SIZE + offset.y + CELL_SIZE / 2, CELL_SIZE / 2 - 5, RED);
            }
        EndDrawing();
    }



    CloseWindow();
    return 0;
}

pos_st getBoardPosFromMousePos(Vector2 mousePos) {
    if (mousePos.x <= offset.x || mousePos.x > (offset.x + WIDTH * CELL_SIZE) || mousePos.y < offset.y || mousePos.y > (offset.y + HEIGHT * CELL_SIZE))
        return (pos_st) {.x = -1, .y = -1};

    return (pos_st) {.x = (mousePos.x - offset.x) / CELL_SIZE, .y = (mousePos.y - offset.y) / CELL_SIZE};
}

bool isOOB(pos_st boardPos) {
    return (boardPos.x < 0 || boardPos.x >= WIDTH || boardPos.y < 0 || boardPos.y >= HEIGHT);
}

bool isBoardPosValid(board_t board, pos_st boardPos) {
    return !isOOB(boardPos) && board[0][boardPos.x] == EMPTY;
}

void printBoard(board_t board) {
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            printf("%d ", board[j][i]);
        }
        printf("\n");
    }
    printf("\n");
}

void updateBoard(board_t board, pos_st boardPos, bool isPlayerTurn, pos_st* lastMove) {
    for (boardPos.y = HEIGHT - 1; board[boardPos.y][boardPos.x]; boardPos.y--);
    board[boardPos.y][boardPos.x] = (isPlayerTurn ? PLAYER : COMPUTER);
    *lastMove = boardPos;
}

int checkAlignment(board_t board, int x, int y, int dx, int dy) {
    int player = board[y][x];
    int nx, ny;

    for (int step = 1; step < 4; step++) {
        nx = x + step * dx;
        ny = y + step * dy;
        if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT || board[ny][nx] != player) {
            return 0;
        }
    }
    return player;
}

int whoWins(board_t board) {
    int winner;
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            if (board[j][i]) {
                if ((winner = checkAlignment(board, i, j, 1, 0)) || 
                    (winner = checkAlignment(board, i, j, 0, 1)) || 
                    (winner = checkAlignment(board, i, j, 1, 1)) || 
                    (winner = checkAlignment(board, i, j, 1, -1)))
                    return winner;
            }
        }
    }
    for (int i = 0; i < 7; i++) {
        if (!board[0][i])
            return 0;
    }
    return 3;
}

pos_st bestMove(board_t board, validCells_st* validCells, int validCellsCount) {
    pos_st bestPos;
    int bestScore;
    
    for (int i = 0; i < validCellsCount; i++) {
        validCells[i].score = evaluateBoard(board, validCells[i].pos);
    }

    bestPos = validCells[0].pos;
    bestScore = validCells[0].score;
    for (int i = 0; i < validCellsCount; i++) {
        if (validCells[i].score > bestScore) {
            bestScore = validCells[i].score;
            bestPos = validCells[i].pos;
        }
    }
    
    return bestPos;
}

int evaluateBoard(board_t board, pos_st pos) {
    int score = 0;
    // copie du plateau et pose du pion de l'ordi
    board_t bcopy;
    memcpy(bcopy, board, sizeof(board_t));
    bcopy[pos.y][pos.x] = COMPUTER;

    // 1) Victoire immédiate pour l'ordi => meilleur score
    if (whoWins(bcopy) == COMPUTER) return 100000;

    // 2) Détecter si on bloque une victoire immédiate du joueur
    bool playerHadImmediateWin = false;
    int blockingBonus = 0;
    for (int col = 0; col < WIDTH; col++) {
        if (board[0][col] != EMPTY) continue;
        int r = HEIGHT - 1;
        while (r > 0 && board[r][col] != EMPTY) r--;
        if (board[r][col] != EMPTY) continue; // sécurité

        board_t tmp;
        memcpy(tmp, board, sizeof(board_t));
        tmp[r][col] = PLAYER;
        if (whoWins(tmp) == PLAYER) {
            playerHadImmediateWin = true;
            // si notre coup place un pion sur cette même case, on bloque
            if (col == pos.x && r == pos.y) {
                blockingBonus += 60000; // très important
            }
        }
    }
    if (blockingBonus) score += blockingBonus;

    // 3) Vérifier si, après notre coup, le joueur a une victoire immédiate -> éviter
    for (int col = 0; col < WIDTH; col++) {
        if (bcopy[0][col] != EMPTY) continue;
        int r = HEIGHT - 1;
        while (r > 0 && bcopy[r][col] != EMPTY) r--;
        if (bcopy[r][col] != EMPTY) continue;

        board_t tmp;
        memcpy(tmp, bcopy, sizeof(board_t));
        tmp[r][col] = PLAYER;
        if (whoWins(tmp) == PLAYER) {
            // si notre coup rend possible une victoire immédiate de l'adversaire, c'est très mauvais
            return -90000;
        }
    }

    // 4) Heuristique: favoriser alignements pour l'ordi et pénaliser ceux du joueur
    // Directions: horizontal, vertical, diag1, diag2
    const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
    for (int d = 0; d < 4; d++) {
        int dx = dirs[d][0];
        int dy = dirs[d][1];

        // compter longueur d'alignement pour l'ordi autour de pos
        int countC = 1;
        for (int step = 1;; step++) {
            int nx = pos.x + step * dx;
            int ny = pos.y + step * dy;
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) break;
            if (bcopy[ny][nx] == COMPUTER) countC++;
            else break;
        }
        for (int step = 1;; step++) {
            int nx = pos.x - step * dx;
            int ny = pos.y - step * dy;
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) break;
            if (bcopy[ny][nx] == COMPUTER) countC++;
            else break;
        }
        // plus l'alignement est long, exponentielle récompense
        score += (countC * countC * 200);

        // idem pour le joueur (pénalité)
        int countP = 0;
        // chercher éventuels alignements déjà existants du joueur (sans notre pion)
        // on compte séquences qui incluent cases adjacentes à notre coup
        for (int step = 1;; step++) {
            int nx = pos.x + step * dx;
            int ny = pos.y + step * dy;
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) break;
            if (board[ny][nx] == PLAYER) countP++;
            else break;
        }
        for (int step = 1;; step++) {
            int nx = pos.x - step * dx;
            int ny = pos.y - step * dy;
            if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) break;
            if (board[ny][nx] == PLAYER) countP++;
            else break;
        }
        // si le joueur a déjà 3 alignés autour de cette colonne, c'est dangereux
        if (countP >= 3) score -= 8000;
        else score -= (countP * countP * 150);
    }

    // 5) Bonus pour jouer au centre
    if (pos.x == WIDTH/2) score += 200;

    // 6) Si on a bloqué une victoire adverse initiale, c'est prioritaire
    if (playerHadImmediateWin && blockingBonus) score += 10000;

    return score;
}

void drawBoard(board_t board) {
    for (int j = 0; j < HEIGHT; j++) {
        for (int i = 0; i < WIDTH; i++) {
            DrawRectangle(i * CELL_SIZE + offset.x, j * CELL_SIZE + offset.y, CELL_SIZE, CELL_SIZE, BLUE);
            Color color = board[j][i] == PLAYER ? YELLOW : board[j][i] == COMPUTER ? RED : WHITE;
            DrawCircle(i * CELL_SIZE + offset.x + CELL_SIZE / 2, j * CELL_SIZE + offset.y + CELL_SIZE / 2, CELL_SIZE / 2 - 5, color);
        }
    }

    for (int i = 0; i < WIDTH; i++) {
        DrawText(TextFormat("%c", i + 'A'), i * CELL_SIZE + offset.x + CELL_SIZE / 2 - 10, CELL_SIZE / 3 + HEIGHT * CELL_SIZE + offset.y, CELL_SIZE / 2, BLACK);
    }
}
