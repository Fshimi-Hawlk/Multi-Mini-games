#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>
#include <time.h>
#include <math.h>

#include "raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define CELL_SIZE 25
#define RGB(r, g, b) (Color) {r, g, b, 255}
#define BACKGROUND_COLOR RGB(0, 0, 30)
#define BOARD_BACKGROUND_COLOR RGB(50, 50, 50)
#define BOARD_GRID_COLOR RGB(100, 100, 100)

#define CYAN RGB(0, 255, 255)

typedef struct iVector2 {
    int x, y;
} iVector2;

typedef iVector2 tetramino[4];
typedef Color board_t[BOARD_HEIGHT][BOARD_WIDTH];

// x, y

#define I_SHAPE { \
    {-1, 0}, {0, 0}, {1, 0}, {2, 0} \
}

#define O_SHAPE { \
    {0, 0}, {1, 0}, \
    {0, 1}, {1, 1} \
}

#define T_SHAPE { \
    {-1, 0}, {0, 0}, {1, 0},  \
             {0, 1} \
}

#define S_SHAPE { \
             {0, 0}, {1, 0},  \
    {-1, 1}, {0, 1} \
}

#define Z_SHAPE { \
    {-1, 0}, {0, 0},  \
             {0, 1}, {1, 1} \
}

#define J_SHAPE { \
             {0, -1}, \
             {0, 0}, \
    {-1, 1}, {0, 1} \
}

#define L_SHAPE { \
    {0, -1}, \
    {0, 0}, \
    {0, 1}, {1, 1} \
}

typedef enum {
    I_SHAPE_ID, 
    O_SHAPE_ID, 
    T_SHAPE_ID, 
    S_SHAPE_ID, 
    Z_SHAPE_ID, 
    J_SHAPE_ID, 
    L_SHAPE_ID,
    SHAPE_MAX_ID
} shapeId;

typedef struct {
    tetramino shape;
    iVector2 position;
    Color color;
    int rotation;
    int shapeName;
} boardShape_st;

typedef struct {
    float t;
    float tDrop;
    float duration;
} speed_st;

typedef struct {
    float leftTimer;
    float rightTimer;
    float downTimer;
    float initialDelay;
    float repeatDelay;
} inputRepeat_st;

typedef struct {
    iVector2 position;
    int rotation;
} moveAlgoResult_st;

inputRepeat_st inputRepeat = {0, 0, 0, 0.2f, 0.05f};

Color tetraminosColors[] = {CYAN, YELLOW, MAGENTA, GREEN, RED, BLUE, ORANGE};
tetramino tetraminosShapes[] = {I_SHAPE, O_SHAPE, T_SHAPE, S_SHAPE, Z_SHAPE, J_SHAPE, L_SHAPE};

void initBoard(board_t board);

bool compareColor(Color c1, Color c2);
bool isColorsEqual(Color c1, Color c2);

void drawBoard(board_t board);
void drawShape(boardShape_st boardShape);
void drawNextShape(boardShape_st boardShape);
void drawInformations(int score, int level, int lineNbTotal, int highScore);
void drawPreview(board_t board, boardShape_st boardShape);

void automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove);

void mouvement(board_t board, boardShape_st* boardShape);

bool areCoordinatesOOB(int x, int y);
bool isPositonOOB(iVector2 position);
bool isOOBAt(boardShape_st boardShape, iVector2 position);
bool isOOB(boardShape_st boardShape);
bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position);
bool isColliding(board_t board, boardShape_st boardShape);

void putShapeInBoard(board_t board, boardShape_st boardShape);

void randomShape(boardShape_st* boardShape);

void rotationCW(boardShape_st* boardShape);
void rotationCCW(boardShape_st* boardShape);

void automaticDrop(speed_st* speed, boardShape_st* boardShape);

void detectFullLines(board_t board, int lineArray[4], int *lineNb);
void clearLines(board_t board, int lineArray[4], int lineNb);
void handleLineClears(board_t board, int lineArray[4], int *lineNb);

void readHighScore(int *highScore);
void whriteHighScore(int highScore, int score);

void copyBoard(board_t src, board_t dest);
int evaluateBoard(board_t board);
int simulateDrop(board_t board, boardShape_st piece, int col);
moveAlgoResult_st findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape);

int main(int argc, char* argv[]) {
    srand(time(NULL));

    bool autoPlay;
    if (argc == 2 && argv[1][0] == '1')
        autoPlay = true;
    else if (argc == 1 || (argc == 2 && argv[1][0] == '0'))
        autoPlay = false;
    else
        exit(1);

    board_t board;
    boardShape_st boardShape, nextBoardShape;

    randomShape(&boardShape);
    randomShape(&nextBoardShape);

    speed_st speed = {.duration = (autoPlay ? 0.01f : 1.0f)};

    int lineArray[4], lineNb, lineNbTotal = 0;
    int points[5] = {0, 40, 100, 300, 1200}, score = 0, level = 0;
    int highScore;
    readHighScore(&highScore);

    initBoard(board);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(60);

    bool hasFoundMove = false;
    moveAlgoResult_st moveAlgoResult;

    while (!WindowShouldClose()) {
        mouvement(board, &boardShape);

        if (autoPlay) {
            if (!hasFoundMove) {
                moveAlgoResult = findBestMove(board, boardShape, nextBoardShape);

                hasFoundMove = true; //!isOOBAt(boardShape, foundMovePosisition); // Fail guard
                // if (!hasFoundMove) printf("Didn't found any suitable position\n");
            }
            else {
                // moveShapeAt(board, &boardShape, foundMovePosisition); // place la pièce
                automaticMovementTo(&speed, &boardShape, moveAlgoResult);
                
                if (isOOB(boardShape) || isColliding(board, boardShape)) {
                    boardShape.position.y--;
                    putShapeInBoard(board, boardShape);

                    boardShape = nextBoardShape;
                    randomShape(&nextBoardShape);

                    // if (isColliding(board, boardShape)) {
                    //     whriteHighScore(highScore, score);
                    //     return 1;
                    // }
                    
                    hasFoundMove = false;
                }
            }
        }
        else {
            automaticDrop(&speed, &boardShape);
            if (isOOB(boardShape) || isColliding(board, boardShape)) {
                boardShape.position.y--;
                putShapeInBoard(board, boardShape);

                boardShape = nextBoardShape;
                randomShape(&nextBoardShape);

                if (isColliding(board, boardShape)) {
                    whriteHighScore(highScore, score);
                    return 1;
                }
            }
        }

            
        handleLineClears(board, lineArray, &lineNb);
        lineNbTotal += lineNb;
        level = fminf(29, lineNbTotal / 10);
        score += points[lineNb] * (level + 1);

        if (!lineNbTotal % 10 && !autoPlay)
            speed.duration = fmaxf(0.3f, 1.0f - 0.025 * level);

        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            drawBoard(board);
            drawPreview(board, boardShape);
            drawShape(boardShape);
            drawNextShape(nextBoardShape);

            drawInformations(score, level, lineNbTotal, highScore);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void initBoard(board_t board) {
    for (int y = 0; y < BOARD_HEIGHT; y++)
        for (int x = 0; x < BOARD_WIDTH; x++)
            board[y][x] = BOARD_BACKGROUND_COLOR; 
} 

bool isColorsEqual(Color c1, Color c2) {
    return compareColor(c1, c2) == 0;
}

bool compareColor(Color c1, Color c2) {
    return memcmp(&c1, &c2, sizeof(Color));
}

void drawBoard(board_t board) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2; 

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, BOARD_GRID_COLOR);
            DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, board[y][x]);
        }
    } 
}

void drawShape(boardShape_st boardShape) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0)
            continue;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, BOARD_GRID_COLOR);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, boardShape.color);
    }
}

void drawNextShape(boardShape_st boardShape) {
    int offsetX = WINDOW_WIDTH * (2.0 / 3);
    int offsetY = WINDOW_HEIGHT * (1.0 / 3);
    int x, y;

    DrawText("Next shape:", offsetX + 60, offsetY - 60, 20, WHITE);

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, BOARD_GRID_COLOR);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, boardShape.color);
    }
}

void drawInformations(int score, int level, int lineNbTotal, int highScore) {
    char scoreText[30];
    char levelText[30];
    char linesText[30];
    char highScoreText[30];

    sprintf(scoreText, "Score: %d", score);
    sprintf(levelText, "Level: %d", level);
    sprintf(linesText, "Lines: %d", lineNbTotal);
    sprintf(highScoreText, "High score: %d", highScore);

    DrawText(scoreText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2, 20, WHITE);
    DrawText(levelText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 30, 20, WHITE);
    DrawText(linesText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 60, 20, WHITE);
    DrawText(highScoreText, WINDOW_WIDTH / 40, WINDOW_HEIGHT / 2 + 120, 20, WHITE);
}

void drawPreview(board_t board, boardShape_st boardShape) {
    int offsetX = (WINDOW_WIDTH - (CELL_SIZE * BOARD_WIDTH)) / 2;
    int offsetY = (WINDOW_HEIGHT - (CELL_SIZE * BOARD_HEIGHT)) / 2;
    int x, y;

    for (; !isColliding(board, boardShape); (boardShape.position.y)++);;
    (boardShape.position.y)--;

    for (int i = 0; i < 4; i++) {
        x = boardShape.shape[i].x + boardShape.position.x;
        y = boardShape.shape[i].y + boardShape.position.y;

        if (y < 0 || y >= BOARD_HEIGHT || x < 0 || x >= BOARD_WIDTH) continue;

        DrawRectangle((offsetX + x * CELL_SIZE) - 1, (offsetY + y * CELL_SIZE) - 1, CELL_SIZE + 2, CELL_SIZE + 2, WHITE);
        DrawRectangle(offsetX + x * CELL_SIZE, offsetY + y * CELL_SIZE, CELL_SIZE, CELL_SIZE, board[y][x]);
    }
}

void mouvement(board_t board, boardShape_st* boardShape) {
    float dt = GetFrameTime();

    // GAUCHE
    if (IsKeyDown(KEY_LEFT)) {
        if (inputRepeat.leftTimer <= 0.0f) {
            boardShape->position.x--;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.x++;
            inputRepeat.leftTimer = (inputRepeat.leftTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.leftTimer -= dt;
        }
    } else {
        inputRepeat.leftTimer = 0.0f; // réinitialise le timer si relâché
    }

    // DROITE
    if (IsKeyDown(KEY_RIGHT)) {
        if (inputRepeat.rightTimer <= 0.0f) {
            boardShape->position.x++;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.x--;
            inputRepeat.rightTimer = (inputRepeat.rightTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.rightTimer -= dt;
        }
    } else {
        inputRepeat.rightTimer = 0.0f;
    }

    // BAS (accélère la chute)
    if (IsKeyDown(KEY_DOWN)) {
        if (inputRepeat.downTimer <= 0.0f) {
            boardShape->position.y++;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.y--;
            inputRepeat.downTimer = (inputRepeat.downTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.downTimer -= dt;
        }
    } else {
        inputRepeat.downTimer = 0.0f;
    }

    // ROTATION
    if (IsKeyPressed(KEY_UP) && boardShape->shapeName != O_SHAPE_ID) {
        rotationCW(boardShape);
        if (isOOB(*boardShape) || isColliding(board, *boardShape))
            rotationCCW(boardShape);
    }
}

bool areCoordinatesOOB(int x, int y) {
    return (y >= BOARD_HEIGHT) || (x < 0) || (x >= BOARD_WIDTH);
}

bool isPositonOOB(iVector2 position) {
    return areCoordinatesOOB(position.x, position.y);
}

bool isOOBAt(boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int x = boardShape.shape[i].x + position.x;
        int y = boardShape.shape[i].y + position.y;

        if (x < 0 || x >= BOARD_WIDTH) return true;
        if (y >= BOARD_HEIGHT) return true;
    }

    return false;
}

bool isOOB(boardShape_st boardShape) {
    return isOOBAt(boardShape, boardShape.position);
}

bool isCollidingAt(board_t board, boardShape_st boardShape, iVector2 position) {
    for (int i = 0; i < 4; i++) {
        int y = boardShape.shape[i].y + position.y;
        int x = boardShape.shape[i].x + position.x;

        if (y < 0 || x < 0 || x >= BOARD_WIDTH)
            continue;

        if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR))
            return true;
    }

    return false;
}

bool isColliding(board_t board, boardShape_st boardShape) {
    return isCollidingAt(board, boardShape, boardShape.position);
}

void putShapeInBoard(board_t board, boardShape_st boardShape) {
    for (int i = 0; i < 4; i++) {
        int shapeCellY = boardShape.shape[i].y + boardShape.position.y;
        int shapeCellX = boardShape.shape[i].x + boardShape.position.x;

        board[shapeCellY][shapeCellX] = boardShape.color;
    }
}

void randomShape(boardShape_st* boardShape) {
    int n = rand() % SHAPE_MAX_ID;
    memcpy(boardShape->shape, tetraminosShapes[n], sizeof(tetramino));
    boardShape->color = tetraminosColors[n];
    boardShape->position = (iVector2){4, 0};
    boardShape->shapeName = n;
}

void rotationCW(boardShape_st* boardShape) {
    int xTemp;
    
    // 90° -> (x, y) => (-y, x)
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = -boardShape->shape[i].y;
        boardShape->shape[i].y = xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 1) % 4;
}

void rotationCCW(boardShape_st* boardShape) {
    int xTemp;
    
    // -90° -> (x, y) => (y, -x)
    for (int i = 0; i < 4; i++) {
        xTemp = boardShape->shape[i].x;
        boardShape->shape[i].x = boardShape->shape[i].y;
        boardShape->shape[i].y = -xTemp;
    }

    boardShape->rotation = (boardShape->rotation + 3) % 4;
}

void automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop < 1) return;

    speed->t = 0.0f;
    speed->tDrop = 0.0f;

    boardShape->position.y++;

    boardShape->position.x += (boardShape->position.x < targetMove.position.x) *  1; // move left 
    boardShape->position.x += (boardShape->position.x > targetMove.position.x) * -1;

    if (targetMove.rotation != boardShape->rotation)
        rotationCW(boardShape);
}

void automaticDrop(speed_st* speed, boardShape_st* boardShape) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop >= 1) {
        speed->t = 0.0f;
        speed->tDrop = 0.0f;
        (boardShape->position.y)++;
    }
}

void detectFullLines(board_t board, int lineArray[4], int *lineNb) {
    int nInLign;
    *lineNb = 0;

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        nInLign = 0;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (memcmp(&board[y][x], &BOARD_BACKGROUND_COLOR, sizeof(BOARD_BACKGROUND_COLOR)))
                nInLign++;
        }
        if (nInLign == BOARD_WIDTH)
            lineArray[(*lineNb)++] = y;
    }
}

void clearLines(board_t board, int lineArray[4], int lineNb) {
    for (int i = 0; i < lineNb; i++) {
        for (int y = lineArray[i] - 1; y >= 0; y--) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[y + 1][x] = board[y][x];
            }
        }
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[0][x] = BOARD_BACKGROUND_COLOR;
        }
    }
}

void handleLineClears(board_t board, int lineArray[4], int *lineNb) {
    detectFullLines(board, lineArray, lineNb);
    clearLines(board, lineArray, *lineNb);
}

void readHighScore(int *highScore) {
    FILE* fd = fopen("data/highScore.txt", "r");

    if (!fd) {
        *highScore = 0;
        return;
    }
    
    fscanf(fd, "%d", highScore);

    fclose(fd);
}

void whriteHighScore(int highScore, int score) {
    if (score <= highScore) return;

    FILE* fd = fopen("data/highScore.txt", "w");

    if (fd) {
        fprintf(fd, "%d", score);

        fclose(fd);
    }
}

void copyBoard(board_t src, board_t dest) {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            dest[y][x] = src[y][x];
        }
    }
}

// Évalue la qualité d’un plateau (simple heuristique)
int evaluateBoard(board_t board) {
    int holes = 0;
    int aggregateHeight = 0;
    int bumpiness = 0;
    int completeLines = 0;
    int maxHeight = 0;

    int columnHeights[BOARD_WIDTH];

    // 1. Hauteurs par colonne
    for (int x = 0; x < BOARD_WIDTH; x++) {
        columnHeights[x] = 0;
        bool blockFound = false;
        for (int y = 0; y < BOARD_HEIGHT; y++) {
            if (!isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
                if (!blockFound) {
                    blockFound = true;
                    columnHeights[x] = BOARD_HEIGHT - y;
                    aggregateHeight += columnHeights[x];
                    if (columnHeights[x] > maxHeight)
                        maxHeight = columnHeights[x];
                }
            }
            else if (blockFound) {
                holes++; // trou sous un bloc
            }
        }
    }

    // 2. Rugosité
    for (int x = 0; x < BOARD_WIDTH - 1; x++) {
        bumpiness += abs(columnHeights[x] - columnHeights[x + 1]);
    }

    // 3. Lignes complètes
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        bool full = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (isColorsEqual(board[y][x], BOARD_BACKGROUND_COLOR)) {
                full = false;
                break;
            }
        }
        if (full)
            completeLines++;
    }

    // 4. Combinaison pondérée
    int score = 0;
    score += -10 * holes;
    score += -5 * aggregateHeight;
    score += -1 * bumpiness;
    score += -5 * maxHeight;
    score += 20 * completeLines;

    return score;
}

// Simule la chute d’une pièce à une colonne donnée
int simulateDrop(board_t board, boardShape_st piece, int col) {
    piece.position = (iVector2) { col, 0 };

    // Descente jusqu’à collision
    while (!isColliding(board, piece)) {
        piece.position.y++;
    }

    return --piece.position.y;
}

// Trouve la meilleure colonne pour jouer la pièce
moveAlgoResult_st findBestMove(board_t board, boardShape_st shape, boardShape_st nextShape) {
    board_t temp;
    int bestScore = -100000;
    moveAlgoResult_st result = {.position = {-1, -1}, .rotation = -1};

    for (int rot = 0; rot < 4; ++rot) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
            int row = simulateDrop(board, shape, col);
            if (row < 0) continue;
            iVector2 foundPosition = {col, row};
            if (isOOBAt(shape, foundPosition)) continue;
            
            for (int rotNext = 0; rotNext < 4; ++rotNext) {
                for (int colNext = 0; colNext < BOARD_WIDTH; colNext++) {
                    int rowNext = simulateDrop(board, nextShape, col);
                    if (rowNext < 0) continue;
                    iVector2 foundPositionNext = {colNext, rowNext};
                    if (isOOBAt(nextShape, foundPositionNext)) continue;
            
                    copyBoard(board, temp);
                    shape.position = foundPosition;
                    nextShape.position = foundPositionNext;
                    putShapeInBoard(temp, shape);
                    putShapeInBoard(temp, nextShape);

                    int score = evaluateBoard(temp);
                    if (score > bestScore) {
                        bestScore = score;
                        result.position = foundPosition;
                        result.rotation = rot;
                    }
                }
                rotationCW(&nextShape);
            }
        }

        rotationCW(&shape);
    }

    return result;
}
