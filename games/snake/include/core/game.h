/**
    @file game.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Core gameplay logic for the Snake mini-game.
*/
#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "utils/userTypes.h"

/**
    @brief Checks if the given coordinates are outside the board boundaries.

    @param[in]  coord       The coordinates to check.
    @return                 True if the coordinates are out of bounds, false otherwise.
*/
bool snake_isOOB(iVector2 coord);

/**
    @brief Checks if the snake has no body parts.

    @param[in]  snake       Pointer to the snake structure.
    @return                 True if the snake is empty, false otherwise.
*/
bool snake_isSnakeEmpty(Snake_St* snake);

/**
    @brief Initializes the snake structure to zero.

    @param[out] snake       Pointer to the snake structure to initialize.
*/
void snake_initSnake(Snake_St* snake);

/**
    @brief Checks if the next head position results in a collision with the snake's body.

    @param[in]  snake       Pointer to the snake structure.
    @param[in]  nextHeadPos The next position of the snake's head.
    @return                 True if there is a self-collision, false otherwise.
*/
bool snake_selfCollision(const Snake_St* const snake, iVector2 nextHeadPos);

/**
    @brief Updates the game board with the current position of the snake's body parts.

    @param[in,out] board    The game board to update.
    @param[in]     snake    Pointer to the snake structure.
*/
void snake_updateBoard(Board_t board, const Snake_St* const snake);

/**
    @brief Initializes the board with grass and places the snake on it.

    @param[in,out] board    The game board to initialize.
    @param[in]     snake    Pointer to the snake structure.
    @return                 0 on success.
*/
int snake_initBoard(Board_t board, const Snake_St* const snake);

/**
    @brief Spawns an apple at a random empty position on the board.

    @param[in,out] board    The game board where the apple will be spawned.
*/
void snake_spawnApple(Board_t board);

/**
    @brief Writes the high score to the record file.

    @param[in]  highScore   The high score to save.
*/
void snake_writeRecord(int highScore);

/**
    @brief Reads the high score from the record file.

    @return                 The stored high score, or 0 if the file doesn't exist.
*/
int snake_readRecord(void);

/**
    @brief Updates the movement direction based on user keyboard input.

    @param[in,out] direction Pointer to the current direction vector.
    @return                  True if the direction was changed, false otherwise.
*/
bool snake_mouvement(iVector2* direction);

/**
    @brief Appends a new body part to the tail of the snake.

    @param[in,out] snake    Pointer to the snake structure.
    @param[in]     vector   The coordinates of the new body part.
*/
void snake_snakeAppend(Snake_St* snake, iVector2 vector);

/**
    @brief Removes the head body part of the snake (used for movement).

    @param[in,out] snake    Pointer to the snake structure.
    @param[out]    vector   Pointer to receive the coordinates of the removed part (may be NULL).
*/
void snake_snakeRemove(Snake_St* snake, iVector2* vector);

/**
    @brief Frees all memory allocated for the snake's body parts.

    @param[in,out] snake    Pointer to the snake structure.
*/
void snake_freeSnake(Snake_St* snake);

#endif // CORE_GAME_H