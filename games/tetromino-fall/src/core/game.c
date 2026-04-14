/**
    @file game.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief Implementation of game logic and movement for Tetris.
*/
#include "core/game.h"
#include "core/board.h"
#include "core/shape.h"
#include "utils/configs.h"
#include <stdio.h>

/**
    @brief Moves a shape automatically towards a target position and rotation.

    @param[in,out] speed        The current game speed/timing state.
    @param[in,out] boardShape   The shape to move.
    @param[in]     targetMove   The target position and rotation.
*/
void automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop < 1) return;

    speed->t = 0.0f;
    speed->tDrop = 0.0f;

    boardShape->position.y++;

    boardShape->position.x += (boardShape->position.x < targetMove.position.x) *  1; // move right
    boardShape->position.x += (boardShape->position.x > targetMove.position.x) * -1; // move left

    if (targetMove.rotation != boardShape->rotation)
        rotationCW(boardShape);
}

/**
    @brief Handles manual player movement based on input.

    @param[in]     board       The current board state for collision checks.
    @param[in,out] boardShape  The shape to move.
*/
void mouvement(board_t board, boardShape_st* boardShape) {
    float dt = GetFrameTime();

    // LEFT
    if (IsKeyDown(KEY_LEFT)) {
        if (inputRepeat.leftTimer <= 0.0f) {
            boardShape->position.x--;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.x++;
            inputRepeat.leftTimer = (inputRepeat.leftTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.leftTimer -= dt;
        }
    } else {
        inputRepeat.leftTimer = 0.0f; // resets timer if released
    }

    // RIGHT
    if (IsKeyDown(KEY_RIGHT)) {
        if (inputRepeat.rightTimer <= 0.0f) {
            boardShape->position.x++;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.x--;
            inputRepeat.rightTimer = (inputRepeat.rightTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.rightTimer -= dt;
        }
    } else {
        inputRepeat.rightTimer = 0.0f;
    }

    // DOWN (accelerates fall)
    if (IsKeyDown(KEY_DOWN)) {
        if (inputRepeat.downTimer <= 0.0f) {
            boardShape->position.y++;
            if (isOOB(*boardShape) || isColliding(board, *boardShape))
                boardShape->position.y--;
            inputRepeat.downTimer = (inputRepeat.downTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.downTimer -= dt;
        }
    } else {
        inputRepeat.downTimer = 0.0f;
    }

    // ROTATION
    if (IsKeyPressed(KEY_UP) && boardShape->shapeName != O_SHAPE_ID) {
        rotationCW(boardShape);
        if (isOOB(*boardShape) || isColliding(board, *boardShape))
            rotationCCW(boardShape);
    }
}

/**
    @brief Reads the high score from a file.

    @param[out] highScore  Pointer to store the high score.
*/
void readHighScore(int *highScore) {
    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "r");

    if (!fd) {
        *highScore = 0;
        return;
    }
    
    /* FIX: check fscanf return value — if the file is empty or malformed,
     * fscanf returns 0 or EOF and *highScore stays uninitialized (UB).
     * Default to 0 on any parse failure. */
    if (fscanf(fd, "%d", highScore) != 1) {
        *highScore = 0;
    }

    fclose(fd);
}

/**
    @brief Writes the score to the high score file if it's a new record.

    @param[in] highScore  Current high score.
    @param[in] score      Current game score.
*/
void writeHighScore(int highScore, int score) {
    if (score <= highScore) return;

    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "w");

    if (fd) {
        fprintf(fd, "%d", score);

        fclose(fd);
    }
}
