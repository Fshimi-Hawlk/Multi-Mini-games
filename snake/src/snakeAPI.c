/**
    @file snakeAPI.c
    @author Fshimi-Hawlk
    @author Léandre BAUDET
    @date 2026-02-07
    @date 2026-03-17
    @brief Implementation of the Snake mini-game.
*/

#include "core/game.h"
#include "ui/game.h"

#include "logger.h"

#include "APIs/generalAPI.h"
#include "snakeAPI.h"

/**
 * @brief Concrete Snake game state
 */
struct SnakeGame_St {
    BaseGame_St base;               // Must be first - allows safe cast to BaseGame_St*

    Snake_St snake;

    Board_t board;
    int highScore;

    iVector2 direction;
    iVector2 nextDirection;
    SnakeAnimationData_St anim;
    iVector2 nextPos;

    bool move;
    int nbApple;
};

/* ────────────────────────────────────────────────────────────────────────────
   Lifecycle implementation
   ──────────────────────────────────────────────────────────────────────────── */

Error_Et snake_freeGameWrapper(void* game) {
    return snake_freeGame((SnakeGame_St**) game);
}

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

    gameRef->highScore = readRecord();
    
    gameRef->direction = (iVector2) {.x = 1, .y = 0}; // Direction initiale : droite

    gameRef->nextDirection = gameRef->direction;

    initSnake(&gameRef->snake);
    snakeAppend(&gameRef->snake, (iVector2) {.x = 5, .y = 10});
    snakeAppend(&gameRef->snake, (iVector2) {.x = 6, .y = 10});
    snakeAppend(&gameRef->snake, (iVector2) {.x = 7, .y = 10});

    gameRef->anim = (SnakeAnimationData_St) {0, 0.2};

    initBoard(gameRef->board, &gameRef->snake);
    spawnApple(gameRef->board);

    log_debug("Snake initialized successfully");
    return OK;
}

Error_Et snake_gameLoop(SnakeGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

    iVector2 previousTailPos;

    if (!game->move) {
        game->move = mouvement(&game->nextDirection);
    }
    
    game->nextPos = (iVector2) {.x = game->snake.tail->coord.x + game->direction.x, .y = game->snake.tail->coord.y + game->direction.y};
    
    if (selfCollision(&game->snake, game->nextPos) || isOOB(game->nextPos)) {
        if (game->nbApple > game->highScore) {
            writeRecord(game->nbApple);
        }

        game->base.running = false;
    } else {
        game->anim.timer += GetFrameTime();
        
        if (game->anim.timer >= game->anim.delay) {
            game->direction = game->nextDirection;
            snakeAppend(&game->snake, game->nextPos);
            updateBoard(game->board, &game->snake);

            if (game->board[game->snake.tail->coord.y][game->snake.tail->coord.x] == GAME_TILE_APPLE) {
                spawnApple(game->board);
                game->nbApple++;
            } else {
                snakeRemove(&game->snake, &previousTailPos);
                game->board[previousTailPos.y][previousTailPos.x] = GAME_TILE_GRASS;
            }

            game->board[game->snake.tail->coord.y][game->snake.tail->coord.x] = GAME_TILE_HEAD;

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
        
        drawBoard(game->board);
        drawSnake(&game->snake, interpolation, game->direction);
        
        DrawText(TextFormat("Score : %d", game->nbApple), CELL_SIZE * SIZE_BOARD + 10, 50, 20, BLACK);
        DrawText(TextFormat("High Score : %d", game->highScore), CELL_SIZE * SIZE_BOARD + 10, 100, 20, BLACK);
    EndDrawing();

    return OK;
}

Error_Et snake_freeGame(SnakeGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    SnakeGame_St* gameRef = *game;

    freeSnake(&gameRef->snake);

    free(gameRef);
    *game = NULL;

    return OK;
}