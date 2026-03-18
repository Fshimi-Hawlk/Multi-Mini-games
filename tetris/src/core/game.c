#include "core/game.h"
#include "core/board.h"
#include "core/shape.h"
#include "utils/configs.h"

void tetris_automaticMovementTo(speed_st* speed, boardShape_st* boardShape, moveAlgoResult_st targetMove) {
    speed->t += GetFrameTime();
    speed->tDrop = fminf(speed->t / speed->duration, 1.0f);
    if (speed->tDrop < 1) return;

    speed->t = 0.0f;
    speed->tDrop = 0.0f;

    boardShape->position.y++;

    boardShape->position.x += (boardShape->position.x < targetMove.position.x) *  1; // move left 
    boardShape->position.x += (boardShape->position.x > targetMove.position.x) * -1;

    if (targetMove.rotation != boardShape->rotation)
        tetris_rotationCW(boardShape);
}

void tetris_mouvement(board_t board, boardShape_st* boardShape) {
    float dt = GetFrameTime();

    // GAUCHE
    if (IsKeyDown(KEY_LEFT)) {
        if (inputRepeat.leftTimer <= 0.0f) {
            boardShape->position.x--;
            if (tetris_isOOB(*boardShape) || tetris_isColliding(board, *boardShape))
                boardShape->position.x++;
            inputRepeat.leftTimer = (inputRepeat.leftTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.leftTimer -= dt;
        }
    } else {
        inputRepeat.leftTimer = 0.0f; // réinitialise le timer si relâché
    }

    // DROITE
    if (IsKeyDown(KEY_RIGHT)) {
        if (inputRepeat.rightTimer <= 0.0f) {
            boardShape->position.x++;
            if (tetris_isOOB(*boardShape) || tetris_isColliding(board, *boardShape))
                boardShape->position.x--;
            inputRepeat.rightTimer = (inputRepeat.rightTimer == 0.0f) ? inputRepeat.initialDelay : inputRepeat.repeatDelay;
        } else {
            inputRepeat.rightTimer -= dt;
        }
    } else {
        inputRepeat.rightTimer = 0.0f;
    }

    // BAS (accélère la chute)
    if (IsKeyDown(KEY_DOWN)) {
        if (inputRepeat.downTimer <= 0.0f) {
            boardShape->position.y++;
            if (tetris_isOOB(*boardShape) || tetris_isColliding(board, *boardShape))
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
        tetris_rotationCW(boardShape);
        if (tetris_isOOB(*boardShape) || tetris_isColliding(board, *boardShape))
            tetris_rotationCCW(boardShape);
    }
}

void tetris_readHighScore(int *highScore) {
    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "r");

    if (!fd) {
        *highScore = 0;
        return;
    }
    
    fscanf(fd, "%d", highScore);

    fclose(fd);
}

void tetris_writeHighScore(int highScore, int score) {
    if (score <= highScore) return;

    FILE* fd = fopen(ASSET_PATH "data/highScore.txt", "w");

    if (fd) {
        fprintf(fd, "%d", score);

        fclose(fd);
    }
}