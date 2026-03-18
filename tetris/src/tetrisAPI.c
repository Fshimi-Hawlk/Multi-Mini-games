/**
 * @file tetris.c
 * @author Fshimi-Hawlk
 * @date 2026-02-07
 * @date 2026-02-18
 * @brief Implementation of the Tetris mini-game.
 *
 * Contains the concrete `TetrisGame_St` definition and the core logic:
 *   - piece generation and movement
 *   - collision & line clearing
 *   - scoring & speed progression
 *   - high-score persistence
 *   - rendering (board, current/next piece, score info)
 *
 * The game follows classic Tetris rules with:
 *   - 7 tetrominoes (I, O, T, S, Z, J, L)
 *   - increasing speed every 10 lines
 *   - standard scoring (40/100/300/1200 points x level multiplier)
 *   - game over on spawn collision
 *
 * @see tetrisAPI.h for the public interface
 * @see core/board.h, core/shape.h for board and tetromino logic
 * @see ui/board.h, ui/shape.h for rendering helpers
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

/**
 * @brief Concrete Tetris game state
 */
struct TetrisGame_St {
    BaseGame_St base;               // Must be first - allows safe cast to BaseGame_St*

    board_t board;

    boardShape_st boardShape;       // Current falling piece
    boardShape_st nextBoardShape;   // Preview of next piece

    speed_st speed;                 // Controls automatic drop timing

    int clearedLines[4];            // Temporary buffer for cleared line indices
    int clearedLineAmount;          // Total lines cleared (used for difficulty)

    int rewardedPointsPerClearedLineCount[5]; // Points per 0/1/2/3/4 lines cleared
    int difficultyMultiplier;       // Increases every 10 lines (affects score & speed)

    int highScore;                  // Loaded from file, updated on game over
};

/* ────────────────────────────────────────────────────────────────────────────
   Lifecycle implementation
   ──────────────────────────────────────────────────────────────────────────── */

Error_Et tetris_freeGameWrapper(void* game) {
    return tetris_freeGame((TetrisGame_St**) game);
}

Error_Et tetris_initGame__full(TetrisGame_St** game, TetrisConfigs_St configs) {
    (void)configs; // Currently unused - future: difficulty, etc.

    *game = malloc(sizeof(TetrisGame_St));
    if (*game == NULL) {
        log_error("Failed to allocate TetrisGame_St");
        return ERROR_ALLOC;
    }

    TetrisGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    gameRef->base.freeGame = tetris_freeGameWrapper;
    gameRef->base.running  = true;

    gameRef->speed.duration = 1.0f;

    // Scoring table (classic values)
    gameRef->rewardedPointsPerClearedLineCount[0] = 0;
    gameRef->rewardedPointsPerClearedLineCount[1] = 40;
    gameRef->rewardedPointsPerClearedLineCount[2] = 100;
    gameRef->rewardedPointsPerClearedLineCount[3] = 300;
    gameRef->rewardedPointsPerClearedLineCount[4] = 1200;

    // Line clearing buffer
    tetris_randomShape(&gameRef->boardShape);
    tetris_randomShape(&gameRef->nextBoardShape);

    tetris_readHighScore(&gameRef->highScore);
    tetris_initBoard(gameRef->board);

    log_debug("Tetris initialized successfully");
    return OK;
}

Error_Et tetris_gameLoop(TetrisGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

    tetris_mouvement(game->board, &game->boardShape);
    tetris_automaticDrop(&game->speed, &game->boardShape);

    if (tetris_isOOB(game->boardShape) || tetris_isColliding(game->board, game->boardShape)) {
        game->boardShape.position.y--;
        tetris_putShapeInBoard(game->board, game->boardShape);

        game->boardShape = game->nextBoardShape;
        tetris_randomShape(&game->nextBoardShape);

        if (tetris_isColliding(game->board, game->boardShape)) {
            tetris_writeHighScore(game->highScore, game->base.score);
            game->base.running = false;
            return OK;
        }
    }

    int clearedCount = 0;
    tetris_handleLineClears(game->board, game->clearedLines, &clearedCount);

    game->clearedLineAmount += clearedCount;
    game->difficultyMultiplier = (int) fminf(29, game->clearedLineAmount / 10.0f);

    // Score = base points × (level + 1)
    if (clearedCount > 0 && clearedCount <= 4) {
        game->base.score += game->rewardedPointsPerClearedLineCount[clearedCount]
                       * (game->difficultyMultiplier + 1);
    }

    // Speed increases every 10 lines (min 0.3 s delay)
    if (game->clearedLineAmount % 10 == 0) {
        game->speed.duration = fmaxf(0.3f, 1.0f - 0.025f * game->difficultyMultiplier);
    }

    BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        DrawFPS(10, 10);

        tetris_drawBoard(game->board);
        tetris_drawPreview(game->board, game->boardShape);
        tetris_drawShape(game->boardShape);
        tetris_drawNextShape(game->nextBoardShape);
        tetris_drawInformations(
            game->base.score, game->difficultyMultiplier,
            game->clearedLineAmount, game->highScore
        );
    EndDrawing();

    return OK;
}

Error_Et tetris_freeGame(TetrisGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    TetrisGame_St* gameRef = *game;

    free(gameRef);
    *game = NULL;

    return OK;
}