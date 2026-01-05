#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"

extern const char *PIECES_NAMES[7];

extern const char *PIECES_COLORS_NAMES[2];

extern const PieceName_et ORDER_NAME[8];

extern const char *WHITE_IMAGES_PATHS[7];
extern const char *BLACK_IMAGES_PATHS[7];

extern Texture2D white_piece_textures[7];
extern Texture2D black_piece_textures[7];

extern const char *DOT_IMAGE_PATH;
extern const char *CIRCLE_IMAGE_PATH;

extern const Color BRIGHT_COLOR;
extern const Color DARK_COLOR;
extern const Color BORDER_COLOR;

extern const Color YELLOW_HIGHLIGHT;
extern const Color RED_HIGHLIGHT;

extern bool running;
extern bool finished;
extern bool patFinished;
extern bool waitingForPromotion;
extern bool finishStalemate;
extern bool moveSimulationRendering;

extern int winner;

extern int playerTurn;

extern int nbMoves;
extern char movesPlayed[NB_MAX_MOVE][7];
extern char *moveMade;
extern bool saveMove;

extern Player_st* whitePlayer;
extern Player_st* blackPlayer;

extern Piece_st* selectionnedPiece;

extern IVec2_st previousMoveCell[2];
extern IVec2_st positionsPossibles[30];
extern int nbPositionsPossibles;

extern Texture2D piecesTextures[PIECES_PER_PLAYER * 2];

extern Texture2D dotTexture;
extern Texture2D circleTexture;

extern int xPromotion;
extern int yPromotion;

extern int xPrint;
extern int yPrint;

#endif