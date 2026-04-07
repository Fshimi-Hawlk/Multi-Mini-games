#include "core/game.h"
#include "core/board.h"
#include "core/shape.h"
#include "utils/configs.h"

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

void writeHighScore(int highScore, int score) {
    if (score <= highScore) return;

    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "w");

    if (fd) {
        fprintf(fd, "%d", score);

        fclose(fd);
    }
}