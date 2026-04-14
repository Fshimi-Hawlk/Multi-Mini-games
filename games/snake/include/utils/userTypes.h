/**
    @file userTypes.h
    @author Léandre BAUDET
    @date 2026-01-12
    @date 2026-04-14
    @brief Core type definitions used throughout the game - especially lobby and mini-game integration.
*/
#ifndef USER_TYPES_H
#define USER_TYPES_H

#include "common.h"

/**
    @brief Types of tiles on the snake game board.
*/
typedef enum {
    GAME_TILE_GRASS,    ///< Empty tile with grass.
    GAME_TILE_HEAD,     ///< Tile occupied by the snake's head.
    GAME_TILE_BODY,     ///< Tile occupied by the snake's body.
    GAME_TILE_APPLE     ///< Tile occupied by an apple.
} GameTile_Et;

/**
    @brief Data for snake animations.
*/
typedef struct {
    f32 timer;          ///< Current time elapsed in the animation.
    f32 delay;          ///< Total delay between frames.
} SnakeAnimationData_St;

/**
    @brief 2D array representing the game board.
*/
typedef int Board_t[SIZE_BOARD][SIZE_BOARD];

/**
    @brief Forward declaration for snake body part.
*/
typedef struct SnakeBodyPart_St SnakeBodyPart_St;

/**
    @brief Structure for a single part of the snake.
*/
struct SnakeBodyPart_St {
    iVector2 coord;             ///< Board coordinates of this part.
    Vector2 renderPos;          ///< Actual rendering position (pixels).
    SnakeBodyPart_St* suivant;  ///< Pointer to the next part.
};

/**
    @brief Main structure for the snake.
*/
typedef struct {
    SnakeBodyPart_St* head;     ///< Pointer to the head part.
    SnakeBodyPart_St* tail;     ///< Pointer to the tail part.
    int bodyLength;             ///< Current length of the snake.
} Snake_St;

#endif // USER_TYPES_H
