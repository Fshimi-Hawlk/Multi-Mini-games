#include "game.h"
#include "global.h"
#include "error.h"
#include "event.h"

int initPlayers(void) {
    blackPlayer = initPlayer(BLACK_IMAGES_PATHS, COLOR_PIECE_BLACK, 0, 1);
    if (!blackPlayer) {
        error("initPlayer");

        return 1;
    }

    whitePlayer = initPlayer(WHITE_IMAGES_PATHS, COLOR_PIECE_WHITE, 7, 6);
    if (!whitePlayer) {
        error("initPlayer");

        freePlayer(blackPlayer);

        return 1;
    }

    return 0;
}

Player_st* initPlayer(const char* ImagesPaths[], ColorPiece_et color, int mainLineY, int pawnLineY) {
    Player_st* player = calloc(1, sizeof(Player_st));
    if (!player) {
        error("Couldn't allocate memory");
        return NULL;
    }

    player->color = color;

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        player->pieces[i] = malloc(sizeof(Piece_st));
        if (!player->pieces[i]) {
            error("Couldn't allocate memory");
            
            for (int j = 0; j < i; j++) {
                free(player->pieces[i]);
            }

            free(player);
            player = NULL;
            
            return NULL;
        }
    }

    for (int i = 0; i < PIECES_PER_PLAYER / 2; i++) {
        player->pieces[i]->name = PIECES_NAMES[i];
        player->pieces[i]->color = color;
        player->pieces[i]->pos.x = i;
        player->pieces[i]->pos.y = mainLineY;
        player->pieces[i]->isTaken = false;
        player->pieces[i]->canRock = false;

        player->pieces[i + BOARD_SIZE]->name = PIECE_NAME_PAWN;
        player->pieces[i + BOARD_SIZE]->color = color;
        player->pieces[i + BOARD_SIZE]->pos.x = i;
        player->pieces[i + BOARD_SIZE]->pos.y = pawnLineY;
        player->pieces[i + BOARD_SIZE]->isTaken = false;
        player->pieces[i + BOARD_SIZE]->canRock = false;
    }

    return player;
}

int initTextures(void) {
    for (int i = 1; i < 7; i++) {
        white_piece_textures[i] = LoadTexture(WHITE_IMAGES_PATHS[i]);
        if (!IsTextureValid(white_piece_textures[i])) {
            error("LoadTexture: '%s'", WHITE_IMAGES_PATHS[i]);

            freeTextures();
        }
        black_piece_textures[i] = LoadTexture(BLACK_IMAGES_PATHS[i]);
        if (!IsTextureValid(black_piece_textures[i])) {
            error("LoadTexture: '%s'", BLACK_IMAGES_PATHS[i]);

            freeTextures();
        }
    }

    dotTexture = LoadTexture(DOT_IMAGE_PATH);
    if (!IsTextureValid(dotTexture)) {
        error("LoadTexture: '%s'", DOT_IMAGE_PATH);
        freeTextures();
        return 1;
    }

    circleTexture = LoadTexture(CIRCLE_IMAGE_PATH);
    if (!IsTextureValid(circleTexture)) {
        error("LoadTexture: '%s'", CIRCLE_IMAGE_PATH);
        freeTextures();
        return 1;
    }

    return 0;
}

void freeTextures(void) {
    for (int i = 1; i < 7; i++) {
        if (IsTextureValid(white_piece_textures[i])) {
            UnloadTexture(white_piece_textures[i]);
        }

        if (IsTextureValid(black_piece_textures[i])) {
            UnloadTexture(black_piece_textures[i]);
        }
    }

    if (IsTextureValid(dotTexture)) {
        UnloadTexture(dotTexture);
    }

    if (IsTextureValid(circleTexture)) {
        UnloadTexture(circleTexture);
    }
}

int initBoard(Board_t board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = NULL;
        }
    }

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        board[blackPlayer->pieces[i]->pos.y][blackPlayer->pieces[i]->pos.x] = blackPlayer->pieces[i];
        board[whitePlayer->pieces[i]->pos.y][whitePlayer->pieces[i]->pos.x] = whitePlayer->pieces[i];
    }

    return 0;
}

int initGame(Board_t board) {
    int returnCode;

    moveMade = calloc(7, sizeof(char));
    if (!moveMade) {
        error("Couldn't allocate memory");
        return 1;
    }

    if ((returnCode = initTextures())) {
        error("Couldn't load the textures, closing... (Error Code: %d)", returnCode);
        free(moveMade);
        return returnCode;
    }

    if ((returnCode = initPlayers())) {
        error("Couldn't load the players, closing... (Error Code: %d)", returnCode);
        free(moveMade);
        freeTextures();
        return returnCode;
    }
    
    returnCode = initBoard(board);

    return 0;
}

void resetGame() {
    previousMoveCell[0].x = -1;
    nbPositionsPossibles = 0;
}

void gameLoop(Board_t board, char *predifinedMoves[], int nbMoves) {    
    if (nbMoves > 0 && predifinedMoves) {
        faitpredifinedMoves(board, predifinedMoves, nbMoves);
    }

    while (running) {
        handleEvents(board);

        if (saveMove) {
            strcpy(movesPlayed[nbMoves++], moveMade);
            saveMove = false;
        }

        if (!finished && (patFinished = pat(board))) {
            finished = true;
            afficherCoupFait();
        }
        else if (!finished && echecMat(board)) {
            finished = true;
            afficherCoupFait();
        }

        renderFrame(board);
    }
}

void freePlayer(Player_st* player) {
    if (!player) {
        return;
    }

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (!player->pieces[i]) continue;

        free(player->pieces[i]);
        player->pieces[i] = NULL;
    }

    free(player);
    player = NULL;
}

void freeGame(Board_t board) {
    freeTextures();

    freePlayer(blackPlayer);
    freePlayer(whitePlayer);
}
