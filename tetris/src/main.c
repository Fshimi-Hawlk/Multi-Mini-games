#include "core/algo.h"
#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"


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

    tetris_randomShape(&boardShape);
    tetris_randomShape(&nextBoardShape);

    speed_st speed = {.duration = (autoPlay ? 0.01f : 1.0f)};

    int lineArray[4], lineNb, lineNbTotal = 0;
    int points[5] = {0, 40, 100, 300, 1200}, score = 0, level = 0;
    int highScore;
    tetris_readHighScore(&highScore);

    tetris_initBoard(board);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetTargetFPS(60);

    bool hasFoundMove = false;
    moveAlgoResult_st moveAlgoResult;

    while (!WindowShouldClose()) {
        tetris_mouvement(board, &boardShape);

        if (autoPlay) {
            if (!hasFoundMove) {
                moveAlgoResult = tetris_findBestMove(board, boardShape, nextBoardShape);

                hasFoundMove = true; //!tetris_tetris_isOOBAt(boardShape, foundMovePosisition); // Fail guard
                // if (!hasFoundMove) printf("Didn't found any suitable position\n");
            }
            else {
                // moveShapeAt(board, &boardShape, foundMovePosisition); // place la pièce
                tetris_automaticMovementTo(&speed, &boardShape, moveAlgoResult);
                
                if (tetris_isOOB(boardShape) || tetris_isColliding(board, boardShape)) {
                    boardShape.position.y--;
                    tetris_putShapeInBoard(board, boardShape);

                    boardShape = nextBoardShape;
                    tetris_randomShape(&nextBoardShape);

                    if (tetris_isColliding(board, boardShape)) {
                        break;
                    }
                    
                    hasFoundMove = false;
                }
            }
        }
        else {
            tetris_automaticDrop(&speed, &boardShape);
            if (tetris_isOOB(boardShape) || tetris_isColliding(board, boardShape)) {
                boardShape.position.y--;
                tetris_putShapeInBoard(board, boardShape);

                boardShape = nextBoardShape;
                tetris_randomShape(&nextBoardShape);

                if (tetris_isColliding(board, boardShape)) {
                    tetris_writeHighScore(highScore, score);
                    return 1;
                }
            }
        }

            
        tetris_handleLineClears(board, lineArray, &lineNb);
        lineNbTotal += lineNb;
        level = fminf(29, lineNbTotal / 10.0f);
        score += points[lineNb] * (level + 1);

        if (!lineNbTotal % 10 && !autoPlay)
            speed.duration = fmaxf(0.3f, 1.0f - 0.025 * level);

        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            tetris_drawBoard(board);
            tetris_drawPreview(board, boardShape);
            tetris_drawShape(boardShape);
            tetris_drawNextShape(nextBoardShape);

            tetris_drawInformations(score, level, lineNbTotal, highScore);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"