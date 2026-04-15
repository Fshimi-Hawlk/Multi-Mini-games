#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

#include "networkInterface.h"
#include "paramsMenu.h"
#include "APIs/generalAPI.h"

static const int SCORE_TABLE[5] = { 0, 40, 100, 300, 1200 };

// Global params menu state for tetris
static ParamsMenu_St tetrominoFall_paramsMenu = {0};

static TetrominoFallGame_St tetrominoFall_game = {0};
static bool seeded = false;

void tetrominoFall_init(void) {
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }

    memset(&tetrominoFall_game, 0, sizeof(tetrominoFall_game));

    tetrominoFall_game.speed.duration = 1.0f;

    tetrominoFall_randomShape(&tetrominoFall_game.boardShape);
    tetrominoFall_randomShape(&tetrominoFall_game.nextBoardShape);

    tetrominoFall_readHighScore(&tetrominoFall_game.highScore);
    tetrominoFall_initBoard(tetrominoFall_game.board);

    // Initialize params menu (settings button)
    paramsMenu_init(&tetrominoFall_paramsMenu);

}

void tetrominoFall_update(float dt) {
    paramsMenu_update(&tetrominoFall_paramsMenu);

    tetrominoFall_mouvement(tetrominoFall_game.board, &tetrominoFall_game.boardShape, dt);
    tetrominoFall_automaticDrop(&tetrominoFall_game.speed, &tetrominoFall_game.boardShape, dt);

    if (tetrominoFall_isOOB(tetrominoFall_game.boardShape) || tetrominoFall_isColliding(tetrominoFall_game.board, tetrominoFall_game.boardShape)) {
        tetrominoFall_game.boardShape.position.y--;
        tetrominoFall_putShapeInBoard(tetrominoFall_game.board, tetrominoFall_game.boardShape);

        tetrominoFall_game.boardShape = tetrominoFall_game.nextBoardShape;
        tetrominoFall_randomShape(&tetrominoFall_game.nextBoardShape);

        if (tetrominoFall_isColliding(tetrominoFall_game.board, tetrominoFall_game.boardShape)) {
            tetrominoFall_writeHighScore(tetrominoFall_game.highScore, tetrominoFall_game.score);
        }
    }

    int clearedCount = 0;
    tetrominoFall_handleLineClears(tetrominoFall_game.board, tetrominoFall_game.clearedLines, &clearedCount);

    tetrominoFall_game.clearedLineAmount += clearedCount;
    tetrominoFall_game.difficultyMultiplier = (int) fminf(29, tetrominoFall_game.clearedLineAmount / 10.0f);

    if (clearedCount > 0 && clearedCount <= 4) {
        tetrominoFall_game.score += SCORE_TABLE[clearedCount] * (tetrominoFall_game.difficultyMultiplier + 1);
    }

    if (clearedCount > 0 && tetrominoFall_game.clearedLineAmount % 10 == 0) {
        tetrominoFall_game.speed.duration = fmaxf(0.3f, 1.0f - 0.025f * tetrominoFall_game.difficultyMultiplier);
    }
}

void tetrominoFall_draw(void) {
    ClearBackground(BACKGROUND_COLOR);
    DrawFPS(10, 10);

    tetrominoFall_drawBoard(tetrominoFall_game.board);
    tetrominoFall_drawPreview(tetrominoFall_game.board, tetrominoFall_game.boardShape);
    tetrominoFall_drawShape(tetrominoFall_game.boardShape);
    tetrominoFall_drawNextShape(tetrominoFall_game.nextBoardShape);

    tetrominoFall_drawInformations(
        tetrominoFall_game.score, 
        tetrominoFall_game.difficultyMultiplier,
        tetrominoFall_game.clearedLineAmount, 
        tetrominoFall_game.highScore
    );

    paramsMenu_draw(&tetrominoFall_paramsMenu);
}

void tetrominoFall_destroy(void) {
    paramsMenu_free(&tetrominoFall_paramsMenu);
}

GameClientInterface_St tetrominoFall_clientInterface = {
    .id = MINI_GAME_ID_TETROMINO_FALL,
    .name = "Tetromino Fall",
    .init = tetrominoFall_init,
    .onData = NULL,
    .update = tetrominoFall_update,
    .draw = tetrominoFall_draw,
    .destroy = tetrominoFall_destroy
};