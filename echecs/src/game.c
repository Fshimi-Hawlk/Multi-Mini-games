/**
 * @file game.c
 * @author Maxime CHAUVEAU
 * @brief Game initialization and management functions for Echecs.
 * @version 1.0
 * @date 2024
 *
 * This file contains functions for initializing the game,
 * managing the game loop, and cleaning up resources.
 */

#include "game.h"
#include "global.h"
#include "error.h"
#include "event.h"
#include "algo.h"
#include "utils.h"
#include "rendering.h"

/**
 * @brief Initialize both players.
 * @return 0 on success, 1 on failure
 */
int initPlayers(void) {
    blackPlayer = initPlayer(COLOR_PIECE_BLACK, 0, 1);
    if (!blackPlayer) {
        error("initPlayer");

        return 1;
    }

    whitePlayer = initPlayer(COLOR_PIECE_WHITE, 7, 6);
    if (!whitePlayer) {
        error("initPlayer");

        freePlayer(blackPlayer);

        return 1;
    }

    return 0;
}

/**
 * @brief Initialize a single player.
 * @param color The color of the player (white or black)
 * @param mainLineY The y-position for the back rank pieces
 * @param pawnLineY The y-position for the pawns
 * @return Pointer to the created player structure, or NULL on failure
 */
Player_st* initPlayer(ColorPiece_et color, int mainLineY, int pawnLineY) {
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
        player->pieces[i]->name = ORDER_NAME[i];
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

/**
 * @brief Load all piece textures from disk.
 * @return 0 on success, 1 on failure
 */
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

/**
 * @brief Free all loaded textures from memory.
 */
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

/**
 * @brief Initialize the board with pieces in starting positions.
 * @param board The board to initialize
 * @return 0 on success
 */
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

/**
 * @brief Initialize the entire game (window, textures, players, board).
 * @param board The board to initialize
 * @return 0 on success, error code on failure
 */
int initGame(Board_t board) {
    int returnCode;

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Echecs");
    SetTargetFPS(60);

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

/**
 * @brief Reset the game state for a new game.
 */
void resetGame() {
    previousMoveCell[0].x = -1;
    nbPositionsPossibles = 0;
}

/**
 * @brief Main game loop that handles input and rendering.
 * @param board The game board
 * @param predifinedMoves Array of predefined moves (can be NULL)
 * @param nbMoves Number of predefined moves
 */
void gameLoop(Board_t board, char *predifinedMoves[], int nbMoves) {    
    if (nbMoves > 0 && predifinedMoves) {
        applyPredifinedMoves(board, predifinedMoves, nbMoves);
    }

    while (running) {
        handleEvents(board);

        if (saveMove) {
            strcpy(movesPlayed[nbMoves++], moveMade);
            saveMove = false;
        }

        if (!finished && (patFinished = isStalemate(board))) {
            finished = true;
            printMovesMade();
        }
        else if (!finished && isCheckmate(board)) {
            finished = true;
            printMovesMade();
        }

        renderFrame(board);
    }
}

/**
 * @brief Free all memory associated with a player.
 * @param player The player to free
 */
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

/**
 * @brief Clean up all game resources.
 */
void freeGame(void) {
    freeTextures();

    freePlayer(blackPlayer);
    freePlayer(whitePlayer);
    CloseWindow();
}
