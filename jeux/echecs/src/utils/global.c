/**
 * @file global.c
 * @author Maxime CHAUVEAU
 * @brief Global variables and constants for the Echecs (Chess) game.
 * @version 1.0
 * @date 2024
 *
 * This file contains all the global variable definitions used
 * throughout the game.
 */

#include "utils/types.h"

/** @brief Array of piece names as strings */
const char *PIECES_NAMES[] = {"None",
    "Pawn",
    "Poney",
    "Rook",
    "Bishop",
    "Queen",
    "King"
};

/** @brief Array of piece color names as strings */
const char *PIECES_COLORS_NAMES[] = {
    "White",
    "Black"
};

/** @brief Default ordering of piece names on the back rank */
const PieceName_et ORDER_NAME[] = {
    PIECE_NAME_ROOK,
    PIECE_NAME_PONEY,
    PIECE_NAME_BISHOP,
    PIECE_NAME_QUEEN,
    PIECE_NAME_KING,
    PIECE_NAME_BISHOP,
    PIECE_NAME_PONEY,
    PIECE_NAME_ROOK
};

/** @brief File paths for white piece images */
const char *WHITE_IMAGES_PATHS[] = {NULL,
    "assets/images/pieces/pawn_w.bmp",
    "assets/images/pieces/poney_w.bmp",
    "assets/images/pieces/rook_w.bmp",
    "assets/images/pieces/bishop_w.bmp",
    "assets/images/pieces/queen_w.bmp",
    "assets/images/pieces/king_w.bmp"
};

/** @brief File paths for black piece images */
const char *BLACK_IMAGES_PATHS[] = {NULL,
    "assets/images/pieces/pawn_b.bmp",
    "assets/images/pieces/poney_b.bmp",
    "assets/images/pieces/rook_b.bmp",
    "assets/images/pieces/bishop_b.bmp",
    "assets/images/pieces/queen_b.bmp",
    "assets/images/pieces/king_b.bmp",
};

/** @brief Textures for white pieces */
Texture2D white_piece_textures[7] = {0};

/** @brief Textures for black pieces */
Texture2D black_piece_textures[7] = {0};

/** @brief File path for the dot overlay texture */
const char *DOT_IMAGE_PATH = "assets/images/UI/dot.bmp";

/** @brief File path for the circle overlay texture */
const char *CIRCLE_IMAGE_PATH = "assets/images/UI/circle.bmp";

/** @brief Bright color for the checkerboard */
const Color BRIGHT_COLOR = {222, 184, 135, 255};

/** @brief Dark color for the checkerboard */
const Color DARK_COLOR = {139, 69, 19, 255};

/** @brief Border color for the board */
const Color BORDER_COLOR = {79, 53, 34, 255};

/** @brief Yellow highlight color for previous move */
const Color YELLOW_HIGHLIGHT = {200, 200, 0, 255};

/** @brief Red highlight color for selected piece */
const Color RED_HIGHLIGHT = {200, 0, 0, 255};

/** @brief Main game loop flag - set to false to exit */
bool running = true;

/** @brief Flag indicating if the game has ended */
bool finished = false;

/** @brief Flag indicating if the game ended in pat (stalemate) */
bool patFinished = false;

/** @brief Flag indicating if waiting for pawn promotion choice */
bool waitingForPromotion = false;

/** @brief Flag indicating if the game ended in stalemate */
bool finishStalemate = false;

/** @brief Flag for move simulation rendering mode */
bool moveSimulationRendering = false;

/** @brief Winner player (0 for white, 1 for black, -1 for none) */
int winner = -1;

/** @brief Current player turn (0 for white, 1 for black) */
int playerTurn = 0; 

/** @brief Array to store all moves played in algebraic notation */
char movesPlayed[NB_MAX_MOVE][7];

/** @brief Number of moves played so far */
int nbMoves = 0;

/** @brief Buffer for the last move made */
char *moveMade = NULL;

/** @brief Flag to indicate a move should be saved */
bool saveMove = false;

/** @brief Pointer to the white player structure */
Player_st* whitePlayer = NULL;

/** @brief Pointer to the black player structure */
Player_st* blackPlayer = NULL;

/** @brief Currently selected piece (NULL if none) */
Piece_st* selectionnedPiece = NULL;

/** @brief Array storing the two cells of the previous move */
IVec2_st previousMoveCell[2];

/** @brief Array of possible positions for selected piece */
IVec2_st positionsPossibles[30];

/** @brief Number of possible positions for selected piece */
int nbPositionsPossibles = 0;

/** @brief Combined array of all piece textures */
Texture2D piecesTextures[PIECES_PER_PLAYER * 2];

/** @brief Texture for the dot overlay */
Texture2D dotTexture;

/** @brief Texture for the circle overlay */
Texture2D circleTexture;

/** @brief X position for promotion menu */
int xPromotion = BOARD_PX_SIZE + CELL_PX_SIZE;

/** @brief Y position for promotion menu */
int yPromotion = 120;

/** @brief X position for printing game status text */
int xPrint = BOARD_PX_SIZE;

/** @brief Y position for printing game status text */
int yPrint = 50;
