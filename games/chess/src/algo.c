/**
    @file algo.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Game logic and algorithms for Chess.
*/
#include "algo.h"
#include "global.h"
#include "utils.h"
#include "rendering.h"
#include "event.h"
#include "error.h"
#include "audio.h"

/**
    @brief Select a piece on the board.
    @param[in,out] board     The game board
    @param[in]     targetPos The position of the piece to select
*/
void selectPiece(Board_t board, IVec2_st targetPos) {
    selectionnedPiece = board[targetPos.y][targetPos.x];
    updatePossibleMoves(board);
} 

/**
    @brief Attempt to move a piece to a new position.
    @param[in,out] board             The game board
    @param[in,out] selectionnedPiece Pointer to the piece being moved
    @param[in]     boardPos          The target coordinates
    @return bool True if the move was successful, false otherwise
*/
bool movement(Board_t board, Piece_st* selectionnedPiece, IVec2_st boardPos) {
    IVec2_st posPIECE_NAME_ROOKDep, posPIECE_NAME_ROOKArr;
    Piece_st* tempPiece = NULL;

    IVec2_st posDeb;

    if (!canBePlaced(board, selectionnedPiece, boardPos.x, boardPos.y)) {
        return false;
    }

    if (!isInCheck(board, selectionnedPiece, boardPos.x, boardPos.y, playerTurn)) {
        selectionnedPiece->canRock = true;

        if (selectionnedPiece->name == PIECE_NAME_KING && ((selectionnedPiece->pos.x == 4 && selectionnedPiece->pos.y == 0 && ((boardPos.x == 2 && boardPos.y == 0) || (boardPos.x == 6 && boardPos.y == 0))) || ((selectionnedPiece->pos.x == 4 && selectionnedPiece->pos.y == 7) && ((boardPos.x == 2 && boardPos.y == 7) || (boardPos.x == 6 && boardPos.y == 7))))) {
            if (boardPos.x == 2 && boardPos.y == 0) {
                posPIECE_NAME_ROOKDep.x = 0;
                posPIECE_NAME_ROOKDep.y = 0;
                posPIECE_NAME_ROOKArr.x = 3;
                posPIECE_NAME_ROOKArr.y = 0;
            }
            else if (boardPos.x == 6 && boardPos.y == 0) {
                posPIECE_NAME_ROOKDep.x = 7;
                posPIECE_NAME_ROOKDep.y = 0;
                posPIECE_NAME_ROOKArr.x = 5;
                posPIECE_NAME_ROOKArr.y = 0;
            }
            else if (boardPos.x == 2 && boardPos.y == 7) {
                posPIECE_NAME_ROOKDep.x = 0;
                posPIECE_NAME_ROOKDep.y = 7;
                posPIECE_NAME_ROOKArr.x = 3;
                posPIECE_NAME_ROOKArr.y = 7;
            }
            else if (boardPos.x == 6 && boardPos.y == 7) {
                posPIECE_NAME_ROOKDep.x = 7;
                posPIECE_NAME_ROOKDep.y = 7;
                posPIECE_NAME_ROOKArr.x = 5;
                posPIECE_NAME_ROOKArr.y = 7;
            }

            tempPiece = board[posPIECE_NAME_ROOKDep.y][posPIECE_NAME_ROOKDep.x];

            board[posPIECE_NAME_ROOKDep.y][posPIECE_NAME_ROOKDep.x] = NULL;
            
            tempPiece->pos.x = posPIECE_NAME_ROOKArr.x;
            tempPiece->pos.y = posPIECE_NAME_ROOKArr.y;

            board[posPIECE_NAME_ROOKArr.y][posPIECE_NAME_ROOKArr.x] = tempPiece;
        }

        board[selectionnedPiece->pos.y][selectionnedPiece->pos.x] = NULL;  

        posDeb = (IVec2_st) {selectionnedPiece->pos.x, selectionnedPiece->pos.y};

        previousMoveCell[0].x = selectionnedPiece->pos.x;
        previousMoveCell[0].y = selectionnedPiece->pos.y;
        
        selectionnedPiece->pos.x = boardPos.x;
        selectionnedPiece->pos.y = boardPos.y;

        sprintf(moveMade, "%c%d%c%d", CBCGC(posDeb.x, posDeb.y), CBCGC(selectionnedPiece->pos.x, selectionnedPiece->pos.y));
        
        previousMoveCell[1].x = selectionnedPiece->pos.x;
        previousMoveCell[1].y = selectionnedPiece->pos.y;
        
        if (board[boardPos.y][boardPos.x]) {
            board[boardPos.y][boardPos.x]->isTaken = true;
        }
        
        board[boardPos.y][boardPos.x] = selectionnedPiece;

        if (selectionnedPiece->name == PIECE_NAME_PAWN && (boardPos.y == 0 || boardPos.y == 7)) {
            waitingForPromotion = true;
            return true;
        }
        
        saveMove = true;

        return true;
    }

    return false;
}

/**
    @brief Check if a piece can be placed at a specific position according to its movement rules.
    @param[in] board             The game board
    @param[in] selectionnedPiece Pointer to the piece to check
    @param[in] col               The target column
    @param[in] row               The target row
    @return bool True if the move is valid, false otherwise
*/
bool canBePlaced(Board_t board, Piece_st* selectionnedPiece, int col, int row) {
    int dx = col - selectionnedPiece->pos.x;
    int dy = row - selectionnedPiece->pos.y;

    int stepX, stepY;
    int x, y;
    int direction;

    if (!dx && !dy)
        return false;

    if (board[row][col] && (selectionnedPiece->color == board[row][col]->color))
        return false;

    switch (selectionnedPiece->name) {
        case PIECE_NAME_PAWN:
            direction = selectionnedPiece->color == COLOR_PIECE_WHITE ? -1 : 1;

            if ((dx == 0) && (dy == direction) && !board[row][col])
                return true;

            if ((dx == 0) && (dy == 2 * direction) && (selectionnedPiece->pos.y == (direction == -1 ? 6 : 1)) && !board[row][col] && !board[row - direction][col])
                return true;

            if ((abs(dx) == 1) && (dy == direction) && board[row][col])
                return true;

            break;

        case PIECE_NAME_ROOK:
            if ((dx == 0) || (dy == 0)) {
                stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

                x = selectionnedPiece->pos.x + stepX;
                y = selectionnedPiece->pos.y + stepY;

                while (x != col || y != row) {
                    if (board[y][x]){
                        return false;
                    }

                    x += stepX;
                    y += stepY;
                }

                return true;
            }

            break;

        case PIECE_NAME_PONEY:
            if (((abs(dx) == 2) && (abs(dy) == 1)) || ((abs(dx) == 1) && (abs(dy) == 2))) {
                return true;
            }

            break;

        case PIECE_NAME_BISHOP:
            if (abs(dx) == abs(dy)) {
                stepX = (dx > 0) ? 1 : -1;
                stepY = (dy > 0) ? 1 : -1;
                x = selectionnedPiece->pos.x + stepX;
                y = selectionnedPiece->pos.y + stepY;
                while (x != col || y != row) {
                    if (board[y][x]) {
                        return false;
                    }

                    x += stepX;
                    y += stepY;
                }

                return true;
            }

            break;

        case PIECE_NAME_QUEEN:
            if ((dx == 0) || (dy == 0) || (abs(dx) == abs(dy))) {
                stepX = (dx == 0) ? 0 : (dx > 0 ? 1 : -1);
                stepY = (dy == 0) ? 0 : (dy > 0 ? 1 : -1);

                x = selectionnedPiece->pos.x + stepX;
                y = selectionnedPiece->pos.y + stepY;

                while (x != col || y != row) {
                    if (board[y][x]) {
                        return false;
                    }

                    x += stepX;
                    y += stepY;
                }


                return true;
            }

            break;

        case PIECE_NAME_KING:
            if ((abs(dx) <= 1) && (abs(dy) <= 1)) {
                return true;
            }

            if (board[0][4] && board[0][7] && !board[0][4]->canRock && !board[0][7]->canRock && col == 6 && row == 0 && !board[0][5] && !board[0][6] && !isSquareThreatened(board, COLOR_PIECE_WHITE, 4, 0) && !isSquareThreatened(board, COLOR_PIECE_WHITE, 5, 0) && !isSquareThreatened(board, COLOR_PIECE_WHITE, 6, 0)) {
                return true;
            }
            if (board[0][4] && board[0][0] && !board[0][4]->canRock && !board[0][0]->canRock && col == 2 && row == 0 && !board[0][1] && !board[0][2] && !board[0][3] && !isSquareThreatened(board, COLOR_PIECE_WHITE, 4, 0) && !isSquareThreatened(board, COLOR_PIECE_WHITE, 2, 0) && !isSquareThreatened(board, COLOR_PIECE_WHITE, 3, 0)) {
                return true;
            }

            if (board[7][4] && board[7][7] && !board[7][4]->canRock && !board[7][7]->canRock && col == 6 && row == 7 && !board[7][5] && !board[7][6] && !isSquareThreatened(board, COLOR_PIECE_BLACK, 4, 7) && !isSquareThreatened(board, COLOR_PIECE_BLACK, 5, 7) && !isSquareThreatened(board, COLOR_PIECE_BLACK, 6, 7)) {
                return true;
            }
            if (board[7][4] && board[7][0] && !board[7][4]->canRock && !board[7][0]->canRock && col == 2 && row == 7 && !board[7][1] && !board[7][2] && !board[7][3] && !isSquareThreatened(board, COLOR_PIECE_BLACK, 4, 7) && !isSquareThreatened(board, COLOR_PIECE_BLACK, 2, 7) && !isSquareThreatened(board, COLOR_PIECE_BLACK, 3, 7)) {
                return true;
            }
            break;
        
        default:
            return false;
    }

    return false;
}

/**
    @brief Check if a move would leave the king in check.
    @param[in,out] board             The game board
    @param[in,out] selectionnedPiece Pointer to the piece to move
    @param[in]     targetColumn      The target column
    @param[in]     targetLine        The target row
    @param[in]     player            The player whose turn it is (0 for white, 1 for black)
    @return bool True if the move would result in check, false otherwise
*/
bool isInCheck(Board_t board, Piece_st* selectionnedPiece, int targetColumn, int targetLine, int player) {
    Player_st* targetPlayer = player ? blackPlayer : whitePlayer;
    Player_st* adversaryPlayer = player ? whitePlayer : blackPlayer;

    if (isOOB(targetColumn, targetLine)) return false;

    Piece_st* targetKing = targetPlayer->pieces[PIECE_ORDER_KING];

    if (!targetKing || targetKing->isTaken) {
        return false;
    }

    Piece_st* adversaryPieceSave = NULL;
    int selectedPieceSaveX, selectedPieceSaveY;

    bool res = 0;

    if (board[targetLine][targetColumn]) {
        adversaryPieceSave = board[targetLine][targetColumn];
        adversaryPieceSave->isTaken = true;
    }

    selectedPieceSaveX = selectionnedPiece->pos.x;
    selectedPieceSaveY = selectionnedPiece->pos.y;

    board[selectionnedPiece->pos.y][selectionnedPiece->pos.x] = NULL;

    board[targetLine][targetColumn] = selectionnedPiece;
    board[targetLine][targetColumn]->pos.x = targetColumn;
    board[targetLine][targetColumn]->pos.y = targetLine;

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (!adversaryPlayer->pieces[i] || adversaryPlayer->pieces[i] == NULL || adversaryPlayer->pieces[i]->isTaken)
            continue;
        if ((res = canBePlaced(board, adversaryPlayer->pieces[i], targetKing->pos.x, targetKing->pos.y))) {
            break; 
        }
    }

    board[selectedPieceSaveY][selectedPieceSaveX] = selectionnedPiece;  
    board[selectedPieceSaveY][selectedPieceSaveX]->pos.x = selectedPieceSaveX;  
    board[selectedPieceSaveY][selectedPieceSaveX]->pos.y = selectedPieceSaveY;  
    
    if (adversaryPieceSave) {
        adversaryPieceSave->isTaken = false;
        board[targetLine][targetColumn] = adversaryPieceSave;
    }
    else {
        board[targetLine][targetColumn] = NULL;
    }

    return res;
}

/**
    @brief Check if the current player is in checkmate.
    @param[in,out] board The game board
    @return bool True if checkmate, false otherwise
*/
bool isCheckmate(Board_t board) {
    Player_st* tempJoueur = !playerTurn ? whitePlayer : blackPlayer;

    if (!isInCheck(board, tempJoueur->pieces[PIECE_ORDER_KING], tempJoueur->pieces[PIECE_ORDER_KING]->pos.x, tempJoueur->pieces[PIECE_ORDER_KING]->pos.y, playerTurn)) {
        return false;
    }

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (tempJoueur->pieces[i] == NULL || tempJoueur->pieces[i]->isTaken) {
            continue;
        }

        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (canBePlaced(board, tempJoueur->pieces[i], k, j))
                    if (!isInCheck(board, tempJoueur->pieces[i], k, j, playerTurn))
                        return false;
            }
        }
    }
    
    PlaySound(sound_checkMate);
    return true;
}

/**
    @brief Check if the current player is in stalemate.
    @param[in,out] board The game board
    @return bool True if stalemate, false otherwise
*/
bool isStalemate(Board_t board) {
    Player_st* tempJoueur = !playerTurn ? whitePlayer : blackPlayer;
    
    if (isInCheck(board, tempJoueur->pieces[PIECE_ORDER_KING], tempJoueur->pieces[PIECE_ORDER_KING]->pos.x, tempJoueur->pieces[PIECE_ORDER_KING]->pos.y, playerTurn)) {
        return false;
    }

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (tempJoueur->pieces[i] == NULL || tempJoueur->pieces[i]->isTaken) {
            continue;
        }
        
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (canBePlaced(board, tempJoueur->pieces[i], k, j))
                    if (!isInCheck(board, tempJoueur->pieces[i], k, j, playerTurn))
                        return false;
            }
        }
    }
    return true;
}

/**
    @brief Check if a square is threatened by any of the opponent's pieces.
    @param[in,out] board   The game board
    @param[in]     playerA The current player (0 for white, 1 for black)
    @param[in]     xCase   The x-coordinate of the square to check
    @param[in]     yCase   The y-coordinate of the square to check
    @return bool True if the square is threatened, false otherwise
*/
bool isSquareThreatened(Board_t board, int playerA, int xCase, int yCase) {
    Player_st* playerAdverse = !playerA ? whitePlayer : blackPlayer;

    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        Piece_st* p = playerAdverse->pieces[i];
        if (p == NULL || p->isTaken) continue;

        // For the king, only check 1-square moves — castling is not an attack
        // and calling canBePlaced for the king here would cause infinite recursion
        // (canBePlaced -> isSquareThreatened -> canBePlaced -> ...)
        if (p->name == PIECE_NAME_KING) {
            int dx = abs(xCase - p->pos.x);
            int dy = abs(yCase - p->pos.y);
            if (dx <= 1 && dy <= 1 && (dx + dy > 0)) return true;
        } else {
            if (canBePlaced(board, p, xCase, yCase)) return true;
        }
    }
    return false;
}

/**
    @brief Update the list of possible moves for the currently selected piece.
    @param[in] board The game board
*/
void updatePossibleMoves(Board_t board) {
    nbPositionsPossibles = 0;

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (canBePlaced(board, selectionnedPiece, x, y)) {
                positionsPossibles[nbPositionsPossibles++] = (IVec2_st) {x, y};
            }
        }
    }
}

/**
    @brief Handle the selection of a piece for pawn promotion.
    @param[in,out] board The game board
    @return bool True if a promotion choice was made, false otherwise
*/
bool promotionChoice(Board_t board) {
    IVec2_st mousePos = GetMousePositionI();

    if (mousePos.y < yPromotion || mousePos.y > yPromotion + CELL_PX_SIZE || mousePos.x < xPromotion || mousePos.x > xPromotion + CELL_PX_SIZE * 4) {
        return false;
    }

    Piece_st* piece = NULL;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[0][i] && board[0][i]->name == PIECE_NAME_PAWN) {
            piece = board[0][i];
            break;
        }
        if (board[7][i] && board[7][i]->name == PIECE_NAME_PAWN) {
            piece = board[7][i];
            break;
        }
        if (i == 7) {
            return false;
        }
    }

    PieceName_et promotionVers;

    int names[PIECES_PER_PLAYER] = {0, 'p', 'P', 't', 'f', 'r', 'R'};

    if (mousePos.x >= xPromotion && mousePos.x <= xPromotion + CELL_PX_SIZE) {
        promotionVers = PIECE_NAME_ROOK;

    }
    else if (mousePos.x >= xPromotion + CELL_PX_SIZE && mousePos.x <= xPromotion + CELL_PX_SIZE * 2) {
        promotionVers = PIECE_NAME_PONEY;

    }
    else if (mousePos.x >= xPromotion + CELL_PX_SIZE * 2 && mousePos.x <= xPromotion + CELL_PX_SIZE * 3) {
        promotionVers = PIECE_NAME_BISHOP;

    }
    else if (mousePos.x >= xPromotion + CELL_PX_SIZE * 3 && mousePos.x <= xPromotion + CELL_PX_SIZE * 4) {
        promotionVers = PIECE_NAME_QUEEN;
    }
    else
        return false;

    piece->name = promotionVers;
    waitingForPromotion = false;

    char temp[32]; // Increased size
    strncpy(temp, moveMade, sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';

    snprintf(moveMade, 7, "%s+%c", temp, names[promotionVers]);
    moveMade[6] = '\0';

    saveMove = true;

    return true;

}

/**
    @brief Apply a series of predefined moves to the board (simulating a game).
    @param[in,out] board          The game board
    @param[in]     coupPredefinis Array of strings representing moves in algebraic notation
    @param[in]     nCoup          Number of moves in the array
*/
void applyPredifinedMoves(Board_t board, char *coupPredefinis[], int nCoup) {
    IVec2_st posSrc, posDest; 
    Piece_st* targetPiece = NULL;

    PieceName_et promotionVers;

    moveSimulationRendering = true;

    renderFrame(board);

    for (int i = 0; i < nCoup; i++) {
        WaitTime(0.5);

        playerTurn = !playerTurn;

        posSrc.x = coupPredefinis[i][0] - 'a';
        posSrc.y = BOARD_SIZE - (coupPredefinis[i][1] - '1') - 1;

        posDest.x = coupPredefinis[i][2] - 'a';
        posDest.y = BOARD_SIZE - (coupPredefinis[i][3] - '1') - 1;


        targetPiece = board[posSrc.y][posSrc.x];

        if (!targetPiece) {
            error("%s:%d are wrong no piece here", coupPredefinis[i], i);
            return;
        }

        movement(board, targetPiece, posDest);

        if (strlen(coupPredefinis[i]) == 6) {
            switch (coupPredefinis[i][5]) {
                case 't':
                    promotionVers = PIECE_NAME_ROOK;
                    break;
                
                case 'p':
                    promotionVers = PIECE_NAME_PONEY;
                    break;
                
                case 'f':
                    promotionVers = PIECE_NAME_BISHOP;
                    break;

                case 'r':
                    promotionVers = PIECE_NAME_QUEEN;
                    break;

                default:
                    break;
            }

            targetPiece->name = promotionVers;
            waitingForPromotion = false;
        }

        renderFrame(board);
    }

    moveSimulationRendering = false;
}
