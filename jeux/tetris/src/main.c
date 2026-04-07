#include "core/algo.h"
#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

/* Score table — identical to tetrisAPI.c, declared locally here because main.c
 * is a standalone binary independent of tetrisAPI. */
static const int SCORE_TABLE[5] = { 0, 40, 100, 300, 1200 };

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

    speed_st speed = { .duration = (autoPlay ? 0.01f : 1.0f) };

    int lineArray[4], lineNb, lineNbTotal = 0;
    int score = 0, level = 0;
    int highScore;
    readHighScore(&highScore);

    tetris_initBoard(board);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME);
    SetTargetFPS(60);

    bool hasFoundMove = false;
    moveAlgoResult_st moveAlgoResult;

    while (!WindowShouldClose()) {
        mouvement(board, &boardShape);

        if (autoPlay) {
            if (!hasFoundMove) {
                moveAlgoResult = findBestMove(board, boardShape, nextBoardShape);
                hasFoundMove = true;
            } else {
                automaticMovementTo(&speed, &boardShape, moveAlgoResult);

                if (isOOB(boardShape) || isColliding(board, boardShape)) {
                    boardShape.position.y--;
                    putShapeInBoard(board, boardShape);

                    boardShape = nextBoardShape;
                    randomShape(&nextBoardShape);

                    if (isColliding(board, boardShape))
                        break;

                    hasFoundMove = false;
                }
            }
        } else {
            automaticDrop(&speed, &boardShape);
            if (isOOB(boardShape) || isColliding(board, boardShape)) {
                boardShape.position.y--;
                putShapeInBoard(board, boardShape);

                boardShape = nextBoardShape;
                randomShape(&nextBoardShape);

                if (isColliding(board, boardShape)) {
                    writeHighScore(highScore, score);
                    break;
                }
            }
        }

        handleLineClears(board, lineArray, &lineNb);
        lineNbTotal += lineNb;
        level = (int) fminf(29, lineNbTotal / 10.0f);

        if (lineNb > 0 && lineNb <= 4)
            score += SCORE_TABLE[lineNb] * (level + 1);

        /* FIX: acceleration condition — the old version evaluated `!lineNbTotal % 10`
         * which always equals 0 or 1 due to operator precedence. */
        if (!autoPlay && lineNbTotal > 0 && lineNbTotal % 10 == 0)
            speed.duration = fmaxf(0.3f, 1.0f - 0.025f * level);

        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            DrawFPS(10, 10);

            drawBoard(board);
            drawPreview(board, boardShape);
            tetris_drawShape(boardShape);
            drawNextShape(nextBoardShape);
            drawInformations(score, level, lineNbTotal, highScore);
        EndDrawing();
    }

    writeHighScore(highScore, score);
    CloseWindow();
    return 0;
}

#define LOGGER_IMPLEMENTATION
#include "logger.h"
