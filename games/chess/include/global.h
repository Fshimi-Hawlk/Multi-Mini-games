/**
    @file global.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Global variables and constants for the chess game.
*/
#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

extern const char *PIECES_NAMES[7];                 ///< Array of piece names as strings
extern const char *PIECES_COLORS_NAMES[2];          ///< Array of piece color names as strings
extern const PieceName_et ORDER_NAME[8];            ///< Default ordering of piece names on the back rank
extern const char *WHITE_IMAGES_PATHS[7];           ///< File paths for white piece images
extern const char *BLACK_IMAGES_PATHS[7];           ///< File paths for black piece images

extern Texture2D white_piece_textures[7];           ///< Textures for white pieces
extern Texture2D black_piece_textures[7];           ///< Textures for black pieces

extern const char *DOT_IMAGE_PATH;                  ///< File path for the dot overlay texture
extern const char *CIRCLE_IMAGE_PATH;               ///< File path for the circle overlay texture

extern const Color BRIGHT_COLOR;                    ///< Bright color for the checkerboard
extern const Color DARK_COLOR;                      ///< Dark color for the checkerboard
extern const Color BORDER_COLOR;                    ///< Border color for the board
extern const Color YELLOW_HIGHLIGHT;                ///< Yellow highlight color for previous move
extern const Color RED_HIGHLIGHT;                   ///< Red highlight color for selected piece

extern bool running;                                ///< Main game loop flag - set to false to exit
extern bool finished;                               ///< Flag indicating if the game has ended
extern bool patFinished;                            ///< Flag indicating if the game ended in pat (stalemate)
extern bool waitingForPromotion;                    ///< Flag indicating if waiting for pawn promotion choice
extern bool finishStalemate;                        ///< Flag indicating if the game ended in stalemate
extern bool moveSimulationRendering;                ///< Flag for move simulation rendering mode

extern int winner;                                  ///< Winner player (0 for white, 1 for black, -1 for none)
extern int playerTurn;                              ///< Current player turn (0 for white, 1 for black)
extern int nbMoves;                                 ///< Number of moves played so far
extern char movesPlayed[NB_MAX_MOVE][7];            ///< Array to store all moves played in algebraic notation
extern char *moveMade;                              ///< Buffer for the last move made
extern bool saveMove;                               ///< Flag to indicate a move should be saved

extern Player_st* whitePlayer;                      ///< Pointer to the white player structure
extern Player_st* blackPlayer;                      ///< Pointer to the black player structure
extern Piece_st* selectionnedPiece;                 ///< Currently selected piece (NULL if none)
extern IVec2_st previousMoveCell[2];                ///< Array storing the two cells of the previous move
extern IVec2_st positionsPossibles[30];             ///< Array of possible positions for selected piece
extern int nbPositionsPossibles;                    ///< Number of possible positions for selected piece

extern Texture2D piecesTextures[PIECES_PER_PLAYER * 2]; ///< Combined array of all piece textures
extern Texture2D dotTexture;                        ///< Texture for the dot overlay
extern Texture2D circleTexture;                     ///< Texture for the circle overlay

extern int xPromotion;                              ///< X position for promotion menu
extern int yPromotion;                              ///< Y position for promotion menu
extern int xPrint;                                  ///< X position for printing game status text
extern int yPrint;                                  ///< Y position for printing game status text

#endif
