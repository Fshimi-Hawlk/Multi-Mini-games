#include "types.h"

const char *PIECES_NAMES[] = {"None",
    "Pawn",
    "Poney",
    "Rook",
    "Bishop",
    "Queen",
    "King"
};

const char *PIECES_COLORS_NAMES[] = {
    "White",
    "Black"
};

const char *WHITE_IMAGES_PATHS[] = {NULL,
    "assets/images/pieces/pawn_w.bmp",
    "assets/images/pieces/poney_w.bmp",
    "assets/images/pieces/rook_w.bmp",
    "assets/images/pieces/bishop_w.bmp",
    "assets/images/pieces/queen_w.bmp",
    "assets/images/pieces/king_w.bmp"
};

const char *BLACK_IMAGES_PATHS[] = {NULL,
    "assets/images/pieces/pawn_b.bmp",
    "assets/images/pieces/poney_b.bmp",
    "assets/images/pieces/rook_b.bmp",
    "assets/images/pieces/bishop_b.bmp",
    "assets/images/pieces/queen_b.bmp",
    "assets/images/pieces/king_b.bmp",
};

const Texture2D WHITE_PIECE_TEXTURES[7] = {0};
const Texture2D BLACK_PIECE_TEXTURES[7] = {0};

const char *DOT_IMAGE_PATH = "assets/images/UI/dot.bmp";
const char *CIRCLE_IMAGE_PATH = "assets/images/UI/circle.bmp";

const Color BRIGHT_COLOR = {222, 184, 135, 255};
const Color DARK_COLOR = {139, 69, 19, 255};
const Color BORDER_COLOR = {79, 53, 34, 255};

const Color YELLOW_HIGHLIGHT = {200, 200, 0, 255};
const Color RED_HIGHLIGHT = {200, 0, 0, 255};

bool running = true;
bool finished = false;
bool patFinished = false;
bool waitingForPromotion = false;
bool finishStalemate = false;
bool moveSimulationRendering = false;

int winner = -1;

int playerTurn = 0; 

int nbMoves = 0;
char movesPlayed[NB_MAX_MOVE][7];
char *moveMade = NULL;
bool saveMove = false;

Player_st* whitePlayer = NULL;
Player_st* blackPlayer = NULL;

Piece_st* selectionnedPiece = NULL;

IVec2_st previousMoveCell[2];
IVec2_st positionsPossibles[30];
int nbPositionsPossibles = 0;

Texture2D piecesTextures[PIECES_PER_PLAYER * 2];

Texture2D dotTexture;
Texture2D circleTexture;