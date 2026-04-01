/**
 * @file tetris.c
 * @author Fshimi-Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Implementation of the Tetris mini-game.
 */

#include "core/board.h"
#include "core/game.h"
#include "core/shape.h"

#include "ui/board.h"
#include "ui/game.h"
#include "ui/shape.h"

#include "utils/configs.h"
#include "utils/types.h"

#include "logger.h"

#include "APIs/generalAPI.h"
#include "tetrisAPI.h"

<<<<<<< HEAD
=======
#include <raylib.h>

// Include params menu header for type definition (not implementation)
#include "../../../firstparty/include/ui/paramsMenu.h"

>>>>>>> 3777fd6 (- add : new 3D golf game)
/* FIX: score table as static constant — removes unnecessary calloc/free for
 * an array of 5 integers known at compile time. */
static const int SCORE_TABLE[5] = { 0, 40, 100, 300, 1200 };

<<<<<<< HEAD
=======
// Global params menu state for tetris
static ParamsMenu_St tetrisParamsMenu = {0};

>>>>>>> 3777fd6 (- add : new 3D golf game)
/**
 * @brief Concrete Tetris game state
 */
struct TetrisGame_St {
    BaseGame_St base;               // Must be first - allows safe cast to BaseGame_St*

    board_t board;

    boardShape_st boardShape;       // Current falling piece
    boardShape_st nextBoardShape;   // Preview of next piece

    speed_st speed;                 // Controls automatic drop timing

    /* FIX: clearedLines as fixed array — maximum size is always 4 (a piece covers
     * at most 4 lines), no need to allocate dynamically. */
    int clearedLines[4];
    int clearedLineAmount;          // Total lines cleared (used for difficulty)

    int difficultyMultiplier;       // Increases every 10 lines (affects score & speed)

    int highScore;                  // Loaded from file, updated on game over
};

/*
   Lifecycle implementation
   */

Error_Et tetris_freeGameWrapper(void* game) {
    return tetris_freeGame((TetrisGame_St**) game);
}

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs) {
    (void)configs;

    *game = malloc(sizeof(TetrisGame_St));
    if (*game == NULL) {
        log_error("Failed to allocate TetrisGame_St");
        return ERROR_ALLOC;
    }

    TetrisGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    gameRef->base.freeGame  = tetris_freeGameWrapper;
    gameRef->base.running   = true;
    gameRef->speed.duration = 1.0f;

    randomShape(&gameRef->boardShape);
    randomShape(&gameRef->nextBoardShape);

    readHighScore(&gameRef->highScore);
    initBoard(gameRef->board);

<<<<<<< HEAD
=======
    // Initialize params menu (settings button)
    paramsMenu_init(&tetrisParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    log_debug("Tetris initialized successfully");
    return OK;
}

Error_Et tetris_gameLoop(TetrisGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

<<<<<<< HEAD
=======
    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return OK;
    }

    // Update params menu (settings button clicks)
    paramsMenu_update(&tetrisParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    mouvement(game->board, &game->boardShape);
    automaticDrop(&game->speed, &game->boardShape);

    if (isOOB(game->boardShape) || isColliding(game->board, game->boardShape)) {
        game->boardShape.position.y--;
        putShapeInBoard(game->board, game->boardShape);

        game->boardShape = game->nextBoardShape;
        randomShape(&game->nextBoardShape);

        if (isColliding(game->board, game->boardShape)) {
            writeHighScore(game->highScore, game->base.score);
            game->base.running = false;
            return OK;
        }
    }

    int clearedCount = 0;
    handleLineClears(game->board, game->clearedLines, &clearedCount);

    game->clearedLineAmount += clearedCount;
    game->difficultyMultiplier = (int) fminf(29, game->clearedLineAmount / 10.0f);

    if (clearedCount > 0 && clearedCount <= 4) {
        game->base.score += SCORE_TABLE[clearedCount] * (game->difficultyMultiplier + 1);
    }

<<<<<<< HEAD
    // Speed updated every 10-line threshold
    if (game->clearedLineAmount % 10 == 0) {
=======
    /* FIX: guard with clearedCount > 0 — without it, the condition
     * (clearedLineAmount % 10 == 0) is true at startup (0 % 10 == 0) and on
     * every frame until the first line is cleared, needlessly rewriting speed. */
    if (clearedCount > 0 && game->clearedLineAmount % 10 == 0) {
>>>>>>> 3777fd6 (- add : new 3D golf game)
        game->speed.duration = fmaxf(0.3f, 1.0f - 0.025f * game->difficultyMultiplier);
    }

    BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawFPS(10, 10);

        drawBoard(game->board);
        drawPreview(game->board, game->boardShape);
        drawShape(game->boardShape);
        drawNextShape(game->nextBoardShape);
        drawInformations(game->base.score, game->difficultyMultiplier,
                         game->clearedLineAmount, game->highScore);
<<<<<<< HEAD
=======

        // Draw params menu (settings button)
        paramsMenu_draw(&tetrisParamsMenu);
>>>>>>> 3777fd6 (- add : new 3D golf game)
    EndDrawing();

    return OK;
}

Error_Et tetris_freeGame(TetrisGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;

<<<<<<< HEAD
=======
    // Cleanup params menu
    paramsMenu_free(&tetrisParamsMenu);

>>>>>>> 3777fd6 (- add : new 3D golf game)
    free(*game);
    *game = NULL;

    return OK;
}
