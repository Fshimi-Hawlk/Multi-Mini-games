/**
    @file global.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Global variables and constants for the Chess game.
*/
#include "types.h"

/**
    @brief Array of piece names as strings.
*/
const char *PIECES_NAMES[] = {"None",
    "Pawn",
    "Poney",
    "Rook",
    "Bishop",
    "Queen",
    "King"
};

/**
    @brief Array of piece color names as strings.
*/
const char *PIECES_COLORS_NAMES[] = {
    "White",
    "Black"
};

/**
    @brief Default ordering of piece names on the back rank.
*/
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

/**
    @brief File paths for white piece images.
*/
const char *WHITE_IMAGES_PATHS[] = {NULL,
    "chess/assets/images/pieces/pawn_w.bmp",
    "chess/assets/images/pieces/poney_w.bmp",
    "chess/assets/images/pieces/rook_w.bmp",
    "chess/assets/images/pieces/bishop_w.bmp",
    "chess/assets/images/pieces/queen_w.bmp",
    "chess/assets/images/pieces/king_w.bmp"
};

/**
    @brief File paths for black piece images.
*/
const char *BLACK_IMAGES_PATHS[] = {NULL,
    "chess/assets/images/pieces/pawn_b.bmp",
    "chess/assets/images/pieces/poney_b.bmp",
    "chess/assets/images/pieces/rook_b.bmp",
    "chess/assets/images/pieces/bishop_b.bmp",
    "chess/assets/images/pieces/queen_b.bmp",
    "chess/assets/images/pieces/king_b.bmp",
};

Texture2D white_piece_textures[7] = {0};   ///< Textures for white pieces
Texture2D black_piece_textures[7] = {0};   ///< Textures for black pieces

const char *DOT_IMAGE_PATH = "chess/assets/images/UI/dot.bmp";     ///< File path for the dot overlay texture
const char *CIRCLE_IMAGE_PATH = "chess/assets/images/UI/circle.bmp"; ///< File path for the circle overlay texture

const Color BRIGHT_COLOR = {222, 184, 135, 255};      ///< Bright color for the checkerboard
const Color DARK_COLOR = {139, 69, 19, 255};          ///< Dark color for the checkerboard
const Color BORDER_COLOR = {79, 53, 34, 255};        ///< Border color for the board
const Color YELLOW_HIGHLIGHT = {200, 200, 0, 255};    ///< Yellow highlight color for previous move
const Color RED_HIGHLIGHT = {200, 0, 0, 255};       ///< Red highlight color for selected piece

bool running = true;             ///< Main game loop flag - set to false to exit
bool finished = false;            ///< Flag indicating if the game has ended
bool patFinished = false;         ///< Flag indicating if the game ended in pat (stalemate)
bool waitingForPromotion = false; ///< Flag indicating if waiting for pawn promotion choice
bool finishStalemate = false;     ///< Flag indicating if the game ended in stalemate
bool moveSimulationRendering = false; ///< Flag for move simulation rendering mode

int winner = -1;                 ///< Winner player (0 for white, 1 for black, -1 for none)
int playerTurn = 0;              ///< Current player turn (0 for white, 1 for black)

/**
    @brief Array to store all moves played in algebraic notation.
*/
char movesPlayed[NB_MAX_MOVE][7];

int nbMoves = 0;                 ///< Number of moves played so far
char *moveMade = NULL;           ///< Buffer for the last move made
bool saveMove = false;           ///< Flag to indicate a move should be saved

Player_st* whitePlayer = NULL;   ///< Pointer to the white player structure
Player_st* blackPlayer = NULL;   ///< Pointer to the black player structure

Piece_st* selectionnedPiece = NULL; ///< Currently selected piece (NULL if none)

/**
    @brief Array storing the two cells of the previous move.
*/
IVec2_st previousMoveCell[2];

/**
    @brief Array of possible positions for selected piece.
*/
IVec2_st positionsPossibles[30];

int nbPositionsPossibles = 0;    ///< Number of possible positions for selected piece

/**
    @brief Combined array of all piece textures.
*/
Texture2D piecesTextures[PIECES_PER_PLAYER * 2];

Texture2D dotTexture;            ///< Texture for the dot overlay
Texture2D circleTexture;         ///< Texture for the circle overlay

int xPromotion = BOARD_PX_SIZE + CELL_PX_SIZE; ///< X position for promotion menu
int yPromotion = 120;                          ///< Y position for promotion menu

int xPrint = BOARD_PX_SIZE;      ///< X position for printing game status text
int yPrint = 50;                 ///< Y position for printing game status text
