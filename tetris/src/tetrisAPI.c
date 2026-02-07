#include "tetrisAPI.h"

#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

#include "logger.h"

struct TetrisGame_St {
    bool running;

    board_t board;
    
    boardShape_st boardShape;
    boardShape_st nextBoardShape;

    speed_st speed;

    int *clearedLines;
    int clearedLineAmount;
    int *rewardedPointsPerClearedLineCount;
    int difficultyMultiplier;
    int score;
    int highScore;
};

TetrisGame_St* tetris_initGame(void) {
    TetrisGame_St* game = calloc(sizeof(*game), 1);

    game->speed.duration = 1.0f;
    
    game->clearedLines = calloc(sizeof(*game->clearedLines), 4);

    game->rewardedPointsPerClearedLineCount = calloc(sizeof(*game->rewardedPointsPerClearedLineCount), 5);
    game->rewardedPointsPerClearedLineCount[0] = 0;
    game->rewardedPointsPerClearedLineCount[1] = 40;
    game->rewardedPointsPerClearedLineCount[2] = 100;
    game->rewardedPointsPerClearedLineCount[3] = 300;
    game->rewardedPointsPerClearedLineCount[4] = 1200;

    game->running = true;

    randomShape(&game->boardShape);
    randomShape(&game->nextBoardShape);
    readHighScore(&game->highScore);

    initBoard(game->board);
    return game;
}

void tetris_gameLoop(TetrisGame_St* const game) {
    if (!game->running) return;

    mouvement(game->board, &game->boardShape);

    automaticDrop(&game->speed, &game->boardShape);
    if (isOOB(game->boardShape) || isColliding(game->board, game->boardShape)) {
        game->boardShape.position.y--;
        putShapeInBoard(game->board, game->boardShape);

        game->boardShape = game->nextBoardShape;
        randomShape(&game->nextBoardShape);

        if (isColliding(game->board, game->boardShape)) {
            writeHighScore(game->highScore, game->score);
            game->running = false;
            return;
        }
    }

    int clearedLineCount = 0;
    handleLineClears(game->board, game->clearedLines, &clearedLineCount);
    game->clearedLineAmount += clearedLineCount;
    game->difficultyMultiplier = fminf(29, game->clearedLineAmount / 10.0f);
    game->score += game->rewardedPointsPerClearedLineCount[clearedLineCount] * (game->difficultyMultiplier + 1);

    if (game->clearedLineAmount % 10 == 0) {
        game->speed.duration = fmaxf(0.3f, 1.0f - 0.025 * game->difficultyMultiplier);
    }

    BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawFPS(10, 10);

        drawBoard(game->board);
        drawPreview(game->board, game->boardShape);
        drawShape(game->boardShape);
        drawNextShape(game->nextBoardShape);

        drawInformations(game->score, game->difficultyMultiplier, game->clearedLineAmount, game->highScore);
    EndDrawing();
}

void tetris_freeGame(TetrisGame_St** game) {
    if (game != NULL) {
        free((*game)->clearedLines);
        (*game)->clearedLines = NULL;

        free((*game)->rewardedPointsPerClearedLineCount);
        (*game)->rewardedPointsPerClearedLineCount = NULL;

        free(*game);
        *game = NULL;
    }
}