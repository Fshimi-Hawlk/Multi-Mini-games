/**
    @file game.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Game initialization and management functions for chess.
*/
#ifndef GAME_H
#define GAME_H

#include "types.h"

/**
    @brief Initialize both players.
    @return 0 on success, 1 on failure
*/
int initPlayers(void);

/**
    @brief Initialize a single player.
    @param[in] color      The color of the player (white or black)
    @param[in] mainLineY  The y-position for the back rank pieces
    @param[in] pawnLineY  The y-position for the pawns
    @return               Pointer to the created player structure, or NULL on failure
*/
Player_st* initPlayer(ColorPiece_et color, int mainLineY, int pawnLineY);

/**
    @brief Load all piece textures from disk.
    @return 0 on success, 1 on failure
*/
int initTextures(void);

/**
    @brief Free all loaded textures from memory.
*/
void freeTextures(void);

/**
    @brief Initialize the board with pieces in starting positions.
    @param[in,out] board  The board to initialize
    @return               0 on success
*/
int initBoard(Board_t board);

/**
    @brief Initialize the entire game (window, textures, players, board).
    @param[in,out] board  The board to initialize
    @return               0 on success, error code on failure
*/
int initGame(Board_t board);

/**
    @brief Reset the game state for a new game.
*/
void resetGame(void);

/**
    @brief Main game loop that handles input and rendering.
    @param[in,out] board              The game board
    @param[in]     predifinedMoves    Array of predefined moves (can be NULL)
    @param[in]     nbMoves            Number of predefined moves
*/
void gameLoop(Board_t board, char *predifinedMoves[], int nbMoves);

/**
    @brief Free all memory associated with a player.
    @param[in,out] player  The player to free
*/
void freePlayer(Player_st* player);

/**
    @brief Clean up all game resources.
*/
void freeGame(void);

#endif
