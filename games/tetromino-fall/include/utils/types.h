/**
    @file types.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Type definitions for the Tetromino Fall game, including shapes, board, and game state structures.
*/
#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include "common.h"

/**
    @brief Array of 4 2D vectors representing the relative positions of blocks in a tetromino.
*/
typedef iVector2 Tetromino_t[4];

/**
    @brief 2D array of colors representing the game board state.
*/
typedef Color Board_t[BOARD_HEIGHT][BOARD_WIDTH];


/**
    @brief Identifiers for each of the seven standard tetromino shapes.
*/
typedef enum {
    I_SHAPE_ID, 
    O_SHAPE_ID, 
    T_SHAPE_ID, 
    S_SHAPE_ID, 
    Z_SHAPE_ID, 
    J_SHAPE_ID, 
    L_SHAPE_ID,
    SHAPE_MAX_ID
} ShapeId_Et;

/**
    @brief State of an active tetromino on the board, including its shape, position, and orientation.
*/
typedef struct {
    Tetromino_t shape;
    iVector2 position;
    Color color;
    int rotation;
    int shapeName;
} BoardShape_St;

/**
    @brief Timing and speed parameters for the game's automatic drop mechanism.
*/
typedef struct {
    float t;
    float tDrop;
    float duration;
} Speed_St;

/**
    @brief Configuration for input repeat delay and frequency (DAS - Delayed Auto Shift).
*/
typedef struct {
    float leftTimer;
    float rightTimer;
    float downTimer;
    float initialDelay;
    float repeatDelay;
} InputRepeat_St;

/**
    @brief Result of the move-finding algorithm, specifying target position and rotation.
*/
typedef struct {
    iVector2 position;
    int rotation;
} MoveAlgoResult_St;

/**
    @brief Concrete Tetromino Fall game state
*/
typedef struct {
    Board_t board;

    BoardShape_St boardShape;       // Current falling piece
    BoardShape_St nextBoardShape;   // Preview of next piece

    Speed_St speed;                 // Controls automatic drop timing

    int clearedLines[4];
    int clearedLineAmount;          // Total lines cleared (used for difficulty)

    int difficultyMultiplier;       // Increases every 10 lines (affects score & speed)

    int score;
    int highScore;                  // Loaded from file, updated on game over
} TetrominoFallGame_St;

#endif
