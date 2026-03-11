/**
 * @file snakeAPI.c
 * @author Fshimi-Hawlk
 * @author Léandre BAUDET
 * @date 2026-02-07
 * @date 2026-03-11
 * @brief Implementation of the Snake mini-game.
 *
 * TODO
 * 
 */

#include "utils/common.h"

#include "logger.h"

#include "APIs/generalAPI.h"
#include "snakeAPI.h"

/**
 * @brief Concrete Snake game state
 */
struct SnakeGame_St {
    BaseGame_St base;               // Must be first - allows safe cast to BaseGame_St*

    Queue_St queue;

    int **board;
    int highScore;

    iVector2 direction;
    iVector2 nextDirection;
    speed_st speed;
    iVector2 nextPos;

    int bodyLength;

    bool move;
    bool gameOver;

    int nbApple;
    iVector2 appleCoord;
    
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

    gameRef->board = calloc(SIZE_BOARD, sizeof(*gameRef->board));
    if (gameRef->board == NULL) {
        log_error("Failed to allocate snake board");
        return ERROR_ALLOC;
    }

    for (int i = 0; i < SIZE_BOARD; ++i) {
        gameRef->board[i] = calloc(SIZE_BOARD, sizeof(**gameRef->board));
        if (gameRef->board[i] == NULL) {
            log_error("Failed to allocate snake board [%d]", i);
            return ERROR_ALLOC;
        }
    }
    
    gameRef->highScore = readRecord();
    
    gameRef->direction = (iVector2) {.x = 1, .y = 0}; // Direction initiale : droite

    gameRef->nextDirection = gameRef->direction;

    initQueue(&gameRef->queue);
    queueAppend(&gameRef->queue, (iVector2){.x = 5, .y = 10});
    queueAppend(&gameRef->queue, (iVector2){.x = 6, .y = 10});
    queueAppend(&gameRef->queue, (iVector2){.x = 7, .y = 10});
    gameRef->bodyLength = 3;

    gameRef->appleCoord;

    gameRef->speed = (speed_st) {0, 0.2};

    initBoard(gameRef->board, gameRef->bodyLength);
    spawnApple(gameRef->board, &gameRef->appleCoord);

    log_debug("Snake initialized successfully");
    return OK;
}

Error_Et snake_gameLoop(SnakeGame_St* const game) {
    if (game == NULL) {
        log_error("NULL game pointer in gameLoop");
        return ERROR_NULL_POINTER;
    }

    if (!game->base.running) return OK;

    iVector2 temp;

    if (!game->gameOver) {
        if (!game->move) {
            game->move = mouvement(&game->nextDirection);
        }
        
        game->nextPos = (iVector2){.x = game->queue.tail->coord.x + game->direction.x, .y = game->queue.tail->coord.y + game->direction.y};
        
        if (selfCollision(game->bodyLength, game->nextPos) || game->nextPos.x < 0 || game->nextPos.x >= SIZE_BOARD || game->nextPos.y < 0 || game->nextPos.y >= SIZE_BOARD) {
            if (game->nbApple > game->highScore) {
                writeRecord(game->nbApple);
            }
            game->gameOver = true;
        }
        else {
            game->speed.timer += GetFrameTime();
            
            if (game->speed.timer >= game->speed.delay) {
                game->direction = game->nextDirection;
                queueAppend(&game->queue, game->nextPos);
                updateBody(game->board, game->bodyLength);

                if (game->board[game->queue.tail->coord.y][game->queue.tail->coord.x] == APPLE) {
                    game->bodyLength++;
                    spawnApple(game->board, &game->appleCoord);
                    game->nbApple++;
                } 
                else {
                    queueRemove(&game->queue, &temp);
                    game->board[temp.y][temp.x] = GRASS;
                }

                game->board[game->queue.tail->coord.y][game->queue.tail->coord.x] = HEAD;

                game->speed.timer = 0;
                game->move = false;
            }
        }

        f32 interpolation = game->speed.timer / game->speed.delay;
        if (interpolation > 1.0f)
            interpolation = 1.0f;

        BeginDrawing();
            ClearBackground(BACKGROUND_COLOR);
            
            drawBoard(game->board);
            drawSnake(game->queue, interpolation, game->direction);
            
            DrawText(TextFormat("Score : %d", game->nbApple), CELL_SIZE * SIZE_BOARD + 10, 50, 20, BLACK);
            DrawText(TextFormat("High Score : %d", game->highScore), CELL_SIZE * SIZE_BOARD + 10, 100, 20, BLACK);
        EndDrawing();
    }

    return OK;
}

Error_Et snake_freeGame(SnakeGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    SnakeGame_St* gameRef = *game;

    freeQueue(&gameRef->queue);

    if (gameRef->board != NULL) {
        for (int i = 0; i < SIZE_BOARD; ++i) {
            if (gameRef->board[i] != NULL) {
                free(gameRef->board[i]);
                gameRef->board[i] = NULL;
            }
        }

        free(gameRef->board);
        gameRef->board = NULL;
    }

    free(gameRef);
    *game = NULL;

    return OK;
}