#include "tetrisAPI.h"

#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

#include "APIs/generalAPI.h"
#include "APIs/tetrisAPI.h"

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

Error_Et tetris_freeGameWrapper(void* game) {
    return tetris_freeGame((TetrisGame_St**) game);
}

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs) {
    (void) configs; // unused

    (*game) = malloc(sizeof(*(*game)));
    if (*game == NULL) return ERROR_ALLOC;

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
    return OK;
}

Error_Et tetris_gameLoop(TetrisGame_St* const game) {
    if (!game->running) return ERROR_NULL_POINTER;

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
            return OK;
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

    return OK;
}

Error_Et tetris_freeGame(TetrisGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    TetrisGame_St* gameRef = *game;

    if (gameRef->clearedLines != NULL) {
        free((*game)->clearedLines);
        (*game)->clearedLines = NULL;

        free((*game)->rewardedPointsPerClearedLineCount);
        (*game)->rewardedPointsPerClearedLineCount = NULL;

        free(*game);
        *game = NULL;
    return OK;
}