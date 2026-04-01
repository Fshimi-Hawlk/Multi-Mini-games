/**
 * @file global.h
 * @author Maxime CHAUVEAU
 * @brief Global variables and constants for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file declares all global variables and constants used
 * throughout the game, including piece names, colors, textures,
 * and game state variables.
 */

#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

/** @brief Array of piece names as strings */
extern const char *PIECES_NAMES[7];

/** @brief Array of piece color names as strings */
extern const char *PIECES_COLORS_NAMES[2];

/** @brief Default ordering of piece names on the back rank */
extern const PieceName_et ORDER_NAME[8];

/** @brief File paths for white piece images */
extern const char *WHITE_IMAGES_PATHS[7];

/** @brief File paths for black piece images */
extern const char *BLACK_IMAGES_PATHS[7];

/** @brief Textures for white pieces */
extern Texture2D white_piece_textures[7];

/** @brief Textures for black pieces */
extern Texture2D black_piece_textures[7];

/** @brief File path for the dot overlay texture */
extern const char *DOT_IMAGE_PATH;

/** @brief File path for the circle overlay texture */
extern const char *CIRCLE_IMAGE_PATH;

/** @brief Bright color for the checkerboard */
extern const Color BRIGHT_COLOR;

/** @brief Dark color for the checkerboard */
extern const Color DARK_COLOR;

/** @brief Border color for the board */
extern const Color BORDER_COLOR;

/** @brief Yellow highlight color for previous move */
extern const Color YELLOW_HIGHLIGHT;

/** @brief Red highlight color for selected piece */
extern const Color RED_HIGHLIGHT;

/** @brief Main game loop flag - set to false to exit */
extern bool running;

/** @brief Flag indicating if the game has ended */
extern bool finished;

/** @brief Flag indicating if the game ended in pat (stalemate) */
extern bool patFinished;

/** @brief Flag indicating if waiting for pawn promotion choice */
extern bool waitingForPromotion;

/** @brief Flag indicating if the game ended in stalemate */
extern bool finishStalemate;

/** @brief Flag for move simulation rendering mode */
extern bool moveSimulationRendering;

/** @brief Winner player (0 for white, 1 for black, -1 for none) */
extern int winner;

/** @brief Current player turn (0 for white, 1 for black) */
extern int playerTurn;

/** @brief Number of moves played so far */
extern int nbMoves;

/** @brief Array to store all moves played in algebraic notation */
extern char movesPlayed[NB_MAX_MOVE][7];

/** @brief Buffer for the last move made */
extern char *moveMade;

/** @brief Flag to indicate a move should be saved */
extern bool saveMove;

/** @brief Pointer to the white player structure */
extern Player_st* whitePlayer;

/** @brief Pointer to the black player structure */
extern Player_st* blackPlayer;

/** @brief Currently selected piece (NULL if none) */
extern Piece_st* selectionnedPiece;

/** @brief Array storing the two cells of the previous move */
extern IVec2_st previousMoveCell[2];

/** @brief Array of possible positions for selected piece */
extern IVec2_st positionsPossibles[30];

/** @brief Number of possible positions for selected piece */
extern int nbPositionsPossibles;

/** @brief Combined array of all piece textures */
extern Texture2D piecesTextures[PIECES_PER_PLAYER * 2];

/** @brief Texture for the dot overlay */
extern Texture2D dotTexture;

/** @brief Texture for the circle overlay */
extern Texture2D circleTexture;

/** @brief X position for promotion menu */
extern int xPromotion;

/** @brief Y position for promotion menu */
extern int yPromotion;

/** @brief X position for printing game status text */
extern int xPrint;

/** @brief Y position for printing game status text */
extern int yPrint;

#endif