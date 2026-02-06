#include "core/algo.h"
#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"
#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"
#include "utils/common.h"
#include "utils/configs.h"
#include "utils/globals.h"
#include "utils/types.h"
#include "utils/utils.h"


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

                    if (isColliding(board, boardShape)) {
                        return 1;
                    }
                    
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
