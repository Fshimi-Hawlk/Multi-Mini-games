/**
    @file game.c
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief Implementation of game logic and movement for Tetris.
*/

#include "core/game.h"
#include "core/board.h"
#include "core/shape.h"

#include "utils/globals.h"

void tetrominoFall_automaticMovementTo(Speed_St* speed, BoardShape_St* boardShape, MoveAlgoResult_St targetMove) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop < 1) return;

    speed->t = 0.0f;
    speed->tDrop = 0.0f;

    boardShape->position.y++;

    boardShape->position.x += (boardShape->position.x < targetMove.position.x) *  1; // move right
    boardShape->position.x += (boardShape->position.x > targetMove.position.x) * -1; // move left

    if (targetMove.rotation != boardShape->rotation)
        tetrominoFall_rotationCW(boardShape);
}

void tetrominoFall_mouvement(Board_t board, BoardShape_St* boardShape, float dt) {
    // LEFT
    if (IsKeyDown(KEY_LEFT)) {
        if (inputRepeat.leftTimer <= 0.0f) {
            boardShape->position.x--;
            if (tetrominoFall_isOOB(*boardShape) || tetrominoFall_isColliding(board, *boardShape))
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
            if (tetrominoFall_isOOB(*boardShape) || tetrominoFall_isColliding(board, *boardShape))
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
            if (tetrominoFall_isOOB(*boardShape) || tetrominoFall_isColliding(board, *boardShape))
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
        tetrominoFall_rotationCW(boardShape);
        if (tetrominoFall_isOOB(*boardShape) || tetrominoFall_isColliding(board, *boardShape))
            tetrominoFall_rotationCCW(boardShape);
    }
}

void tetrominoFall_readHighScore(int *highScore) {
    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "r");

    if (!fd) {
        *highScore = 0;
        return;
    }
    
    if (fscanf(fd, "%d", highScore) != 1) {
        *highScore = 0;
    }

    fclose(fd);
}

void tetrominoFall_writeHighScore(int highScore, int score) {
    if (score <= highScore) return;

    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "w");

    if (fd) {
        fprintf(fd, "%d", score);

        fclose(fd);
    }
}
