/**
    @file snakeAPI.c
    @author Léandre BAUDET
    @date 2026-02-07
    @date 2026-04-14
    @brief Implementation of the Snake mini-game.
*/
#include "core/game.h"
#include "ui/game.h"

#include "logger.h"

#include "APIs/generalAPI.h"
#include "snakeAPI.h"

/**
    @brief Concrete Snake game state
*/
struct SnakeGame_St {
    BaseGame_St base;               ///< Common game interface - must be first member

    Snake_St snake;                 ///< Snake body and metadata

    Board_t board;                  ///< Current game board tiles
    int highScore;                  ///< Player's record score read from disk

    iVector2 direction;             ///< Current movement direction
    iVector2 nextDirection;         ///< Buffered direction for the next move
    SnakeAnimationData_St anim;     ///< Animation timers and configuration
    iVector2 nextPos;               ///< Pre-calculated next position

    bool move;                      ///< Flag indicating if a movement was requested this frame
    int nbApple;                    ///< Number of apples collected in current session
};

/* ────────────────────────────────────────────────────────────────────────────
   Lifecycle implementation
   ──────────────────────────────────────────────────────────────────────────── */

/**
    @brief Wrapper for snake_freeGame to match the generic void* signature.

    @param[in,out] game      Pointer to the game handle (passed as void*)
    @return                  OK on success, ERROR_NULL_POINTER if game is invalid
*/
Error_Et snake_freeGameWrapper(void* game) {
    return snake_freeGame((SnakeGame_St**) game);
}

/**
    @brief Allocates and initializes a new instance of the Snake mini-game.

    @param[out] game         Double pointer to receive the new game handle.
    @param[in]  configs      Configuration parameters (currently unused).
    @return                  OK on success, ERROR_ALLOC on memory failure
*/
Error_Et snake_initGame__full(SnakeGame_St** game, SnakeConfigs_St configs) {
    (void)configs; // Currently unused - future: difficulty, etc.

    *game = calloc(1, sizeof(SnakeGame_St));
    if (*game == NULL) {
        log_error("Failed to allocate SnakeGame_St");
        return ERROR_ALLOC;
    }

    SnakeGame_St* gameRef = *game;

    gameRef->base.freeGame = snake_freeGameWrapper;
    gameRef->base.running  = true;

    gameRef->highScore = snake_readRecord();
    
    gameRef->direction = (iVector2) {.x = 1, .y = 0}; // Direction initiale : droite

    gameRef->nextDirection = gameRef->direction;

    snake_initSnake(&gameRef->snake);
    snake_snakeAppend(&gameRef->snake, (iVector2) {.x = 5, .y = 10});
    snake_snakeAppend(&gameRef->snake, (iVector2) {.x = 6, .y = 10});
    snake_snakeAppend(&gameRef->snake, (iVector2) {.x = 7, .y = 10});

    gameRef->anim = (SnakeAnimationData_St) {0, 0.2};

    snake_initBoard(gameRef->board, &gameRef->snake);
    snake_spawnApple(gameRef->board);

    log_debug("Snake initialized successfully");
    return OK;
}

/**
    @brief Executes one frame of the Snake game (input, update, render).

    @param[in,out] game      The game instance handle.
    @return                  OK on success, ERROR_NULL_POINTER if game is NULL
*/
Error_Et snake_gameLoop(SnakeGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

    // ESC key returns to lobby
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false;
        return OK;
    }

    iVector2 previousTailPos;

    if (!game->move) {
        game->move = snake_mouvement(&game->nextDirection);
    }
    
    iVector2 nextPos = {
        .x = game->snake.tail->coord.x + game->direction.x, 
        .y = game->snake.tail->coord.y + game->direction.y
    };
    
    if (snake_selfCollision(&game->snake, nextPos) || snake_isOOB(nextPos)) {
        if (game->nbApple > game->highScore) {
            snake_writeRecord(game->nbApple);
        }

        game->base.running = false;

    } else {
        game->anim.timer += GetFrameTime();
        
        if (game->anim.timer >= game->anim.delay) {
            game->direction = game->nextDirection;
            snake_snakeAppend(&game->snake, nextPos);

            if (game->board[nextPos.y][nextPos.x] == GAME_TILE_APPLE) {
                snake_spawnApple(game->board);
                game->nbApple++;

            } else {
                snake_snakeRemove(&game->snake, &previousTailPos);
                game->board[previousTailPos.y][previousTailPos.x] = GAME_TILE_GRASS;
            }

            game->board[game->snake.tail->coord.y][game->snake.tail->coord.x] = GAME_TILE_HEAD;

            snake_updateBoard(game->board, &game->snake);

            game->anim.timer = 0;
            game->move = false;
        }
    }

    f32 interpolation = game->anim.timer / game->anim.delay;
    if (interpolation > 1.0f) {
        interpolation = 1.0f;
    }

    BeginDrawing();
        ClearBackground(APP_BACKGROUND_COLOR);
        
        snake_drawBoard(game->board);
        snake_drawSnake(&game->snake, interpolation, game->direction);
        
        DrawText(TextFormat("Score : %d", game->nbApple), CELL_SIZE * SIZE_BOARD + 10, 50, 20, BLACK);
        DrawText(TextFormat("High Score : %d", game->highScore), CELL_SIZE * SIZE_BOARD + 10, 100, 20, BLACK);
    EndDrawing();

    return OK;
}

/**
    @brief Releases all resources and memory associated with the Snake game.

    @param[in,out] game      Pointer to the game handle to be freed and nulled.
    @return                  OK on success, ERROR_NULL_POINTER if game handle is invalid
*/
Error_Et snake_freeGame(SnakeGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    SnakeGame_St* gameRef = *game;

    snake_freeSnake(&gameRef->snake);

    free(gameRef);
    *game = NULL;

    return OK;
}

/**
    @brief Checks if the Snake game is still running.

    @param[in] game      Game instance handle (may be NULL).
    @return              true if game is valid and running, false otherwise.
*/
bool snake_isRunning(const SnakeGame_St* game) {
    return (game != NULL && game->base.running);
}