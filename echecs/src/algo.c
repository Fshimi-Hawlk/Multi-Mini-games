#include "algo.h"
#include "global.h"
#include "error.h"

void selectPiece(Board_t board, IVec2_st targetPos) {
    selectionnedPiece = board[targetPos.y][targetPos.x];
    updatePossibleMoves(board);
} 

bool movement(Board_t board, IVec2_st boardPos) {
    IVec2_st posTourDep, posTourArr;
    Piece_st* tempPiece = NULL;

    IVec2_st posDeb;

    if (!canBePlaced(board, selectionnedPiece, boardPos.x, boardPos.y)) {
        return false;
    }

    if (!isInCheck(board, selectionnedPiece, boardPos.x, boardPos.y, playerTurn)) {
        selectionnedPiece->canRock = true;

        if (selectionnedPiece->name == PIECE_NAME_KING && ((selectionnedPiece->pos.x == 4 && selectionnedPiece->pos.y == 0 && ((boardPos.x == 2 && boardPos.y == 0) || (boardPos.x == 6 && boardPos.y == 0))) || ((selectionnedPiece->pos.x == 4 && selectionnedPiece->pos.y == 7) && ((boardPos.x == 2 && boardPos.y == 7) || (boardPos.x == 6 && boardPos.y == 7))))) {
            if (boardPos.x == 2 && boardPos.y == 0) {
                posTourDep.x = 0;
                posTourDep.y = 0;
                posTourArr.x = 3;
                posTourArr.y = 0;
            }
            else if (boardPos.x == 6 && boardPos.y == 0) {
                posTourDep.x = 7;
                posTourDep.y = 0;
                posTourArr.x = 5;
                posTourArr.y = 0;
            }
            else if (boardPos.x == 2 && boardPos.y == 7) {
                posTourDep.x = 0;
                posTourDep.y = 7;
                posTourArr.x = 3;
                posTourArr.y = 7;
            }
            else if (boardPos.x == 6 && boardPos.y == 7) {
                posTourDep.x = 7;
                posTourDep.y = 7;
                posTourArr.x = 5;
                posTourArr.y = 7;
            }

            tempPiece = board[posTourDep.y][posTourDep.x];

            board[posTourDep.y][posTourDep.x] = NULL;
            
            tempPiece->pos.x = posTourArr.x;
            tempPiece->pos.y = posTourArr.y;

            board[posTourArr.y][posTourArr.x] = tempPiece;
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

bool canBePlaced(Board_t board, Piece_st* selectionnedPiece, int col, int row) {
    int dx = col - selectionnedPiece->pos.x;
    int dy = row - selectionnedPiece->pos.y;

    int stepX, stepY;
    int x, y;
    int direction;

    if (!dx && !dy)
        return false;

    // Ne peut pas manger une pièce de même color
    if (board[row][col] && (selectionnedPiece->color == board[row][col]->color))
        return false;

    switch (selectionnedPiece->name) {
        case PIECE_NAME_PAWN:
            // PIECE_NAME_PAWNs blancs avancent vers le haut, noirs vers le bas
            direction = selectionnedPiece->color == COLOR_PIECE_WHITE ? -1 : 1;

            // Avancer d'une case
            if ((dx == 0) && (dy == direction) && !board[row][col]) // ou !dx && dy == direction && !board[row][col]->name parce que VIDE => 0
                return true;

            // Avancer de deux cases au départ
            if ((dx == 0) && (dy == 2 * direction) && (selectionnedPiece->pos.y == (direction == -1 ? 6 : 1)) && !board[row][col] && !board[row - direction][col])
                return true;

            // Capture en diagonale
            if ((abs(dx) == 1) && (dy == direction) && board[row][col])
                return true;

            break;

        case TOUR:
            // Se déplace en rowne dKINGte (horizontal ou vertical)
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

        case PONEY:
            // Déplacement en L : (±2, ±1) ou (±1, ±2)
            if (((abs(dx) == 2) && (abs(dy) == 1)) || ((abs(dx) == 1) && (abs(dy) == 2))) {
                return true;
            }

            break;

        case FOU:
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

        case REINE:
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

        case KING:
            // Se déplace d'une case dans n'importe quelle direction
            if ((abs(dx) <= 1) && (abs(dy) <= 1)) {
                return true;
            }

            // petit rock noir
            if (board[0][4] && board[0][7] && !board[0][4]->canRock && !board[0][7]->canRock && col == 6 && row == 0 && !board[0][5] && !board[0][6] && !caseMenacee(board, COLOR_PIECE_WHITE, 4, 0) && !caseMenacee(board, COLOR_PIECE_WHITE, 5, 0) && !caseMenacee(board, COLOR_PIECE_WHITE, 6, 0)) {
                return true;
            }
            // grand rock noir
            if (board[0][4] && board[0][0] && !board[0][4]->canRock && !board[0][0]->canRock && col == 2 && row == 0 && !board[0][1] && !board[0][2] && !board[0][3] && !caseMenacee(board, COLOR_PIECE_WHITE, 4, 0) && !caseMenacee(board, COLOR_PIECE_WHITE, 2, 0) && !caseMenacee(board, COLOR_PIECE_WHITE, 3, 0)) {
                return true;
            }

            // petit rock blanc
            if (board[7][4] && board[7][7] && !board[7][4]->canRock && !board[7][7]->canRock && col == 6 && row == 7 && !board[7][5] && !board[7][6] && !caseMenacee(board, NOIRE, 4, 7) && !caseMenacee(board, NOIRE, 5, 7) && !caseMenacee(board, NOIRE, 6, 7)) {
                return true;
            }
            // grand rock blanc
            if (board[7][4] && board[7][0] && !board[7][4]->canRock && !board[7][0]->canRock && col == 2 && row == 7 && !board[7][1] && !board[7][2] && !board[7][3] && !caseMenacee(board, NOIRE, 4, 7) && !caseMenacee(board, NOIRE, 2, 7) && !caseMenacee(board, NOIRE, 3, 7)) {
                return true;
            }
            break;
        
        default:
            return false;
    }

    return false;
}

bool estEnEchec(Board_t board, Piece_st* selectionnedPiece, int targetColumn, int targetLine, int joueur) {
    Player_st* targetPlayer = joueur ? blackPlayer : whitePlayer; // target player est le joueur qui fait le coup
    Player_st* adversaryPlayer = joueur ? whitePlayer : blackPlayer; // adversaryPlayer est le joueur avec qui on va tester pour savoir si il met en echec le targetPlayer

    Piece_st* targetKing = targetPlayer->pieces[4];

    Piece_st* adversaryPieceSave = NULL;
    int selectedPieceSaveX, selectedPieceSaveY;

    bool res = 0;

    // Sauvegarde manuelle de la case ciblée et par extension, et s'il en y a, les données de la pièce ci-trouvant
    if (board[targetLine][targetColumn]) {
        adversaryPieceSave = board[targetLine][targetColumn];
        adversaryPieceSave->isTaken = true;
    }

    selectedPieceSaveX = selectionnedPiece->pos.x;
    selectedPieceSaveY = selectionnedPiece->pos.y;

    // Vide la case de la pièce sélectionnée
    board[selectionnedPiece->pos.y][selectionnedPiece->pos.x] = NULL;

    // Déplacement (temporaire) de la pièce sélectionnée
    board[targetLine][targetColumn]    = selectionnedPiece;
    board[targetLine][targetColumn]->pos.x = targetColumn;
    board[targetLine][targetColumn]->pos.y = targetLine;

    // Vérifier si une pièce adverse peut attaquer le KING
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (adversaryPlayer->pieces[i]->isTaken) 
            continue;

        // Vérifier si la pièce adverse peut attaquer le KING
        if ((res = canBePlaced(board, adversaryPlayer->pieces[i], targetKing->pos.x, targetKing->pos.y))) {
            // Le KING est en échec
            break; 
        }
    }

    // Annulation du mouvement
    board[selectedPieceSaveY][selectedPieceSaveX]    = selectionnedPiece;  
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

bool echecMat(Board_t board) {
    Player_st* tempJoueur = !playerTurn ? whitePlayer : blackPlayer;

    if (!estEnEchec(board, tempJoueur->pieces[PIECE_ORDER_KING], tempJoueur->pieces[PIECE_ORDER_KING]->pos.x, tempJoueur->pieces[PIECE_ORDER_KING]->pos.y, playerTurn)) {
        return false;
    }

    // vérifie si le joueur peut jouer un coup qui le sort de l'échec
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (tempJoueur->pieces[i]->isTaken) {
            continue;
        }

        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (canBePlaced(board, tempJoueur->pieces[i], k, j))
                    if (!estEnEchec(board, tempJoueur->pieces[i], k, j, playerTurn))
                        return false;
            }
        }
    }
    return true;
}

bool pat(Board_t board) {
    Player_st* tempJoueur = !playerTurn ? whitePlayer : blackPlayer;
    
    if (estEnEchec(board, tempJoueur->pieces[PIECE_ORDER_KING], tempJoueur->pieces[PIECE_ORDER_KING]->pos.x, tempJoueur->pieces[PIECE_ORDER_KING]->pos.y, playerTurn)) {
        return false;
    }

    // vérifie si le joueur peut jouer un coup qui le sort de l'échec
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (tempJoueur->pieces[i]->isTaken) {
            continue;
        }
        
        for (int j = 0; j < BOARD_SIZE; j++) {
            for (int k = 0; k < BOARD_SIZE; k++) {
                if (canBePlaced(board, tempJoueur->pieces[i], k, j))
                    if (!estEnEchec(board, tempJoueur->pieces[i], k, j, playerTurn))
                        return false;
            }
        }
    }
    return true;
}

bool caseMenacee(Board_t board, int joueurA, int xCase, int yCase) {
    Player_st* joueurAdverse = !joueurA ? whitePlayer : blackPlayer;

    // vérifie si le joueur peut jouer un coup qui le sort de l'échec
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        if (!joueurAdverse->pieces[i]->isTaken && joueurAdverse->pieces[i]->name != KING && canBePlaced(board, joueurAdverse->pieces[i], xCase, yCase)) {
            return true;
        }
    }
    return false;
}

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

void choixPromotion(SDL_Event event, Board_t board) {
    int mouseX = event.button.x;
    int mouseY = event.button.y;

    if (mouseY < 120 || mouseY > 120 + TAILLE_CASE || mouseX < 680 || mouseX > 680 + TAILLE_CASE * 4) {
        return;
    }

    Piece_st* PIECE_NAME_PAWN;
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[0][i] && board[0][i]->name == PIECE_NAME_PAWN) {
            PIECE_NAME_PAWN = board[0][i];
            break;
        }
        if (board[7][i] && board[7][i]->name == PIECE_NAME_PAWN) {
            PIECE_NAME_PAWN = board[7][i];
            break;
        }
        if (i == 7) {
            return;
        }
    }

    const char *(*colorChemin) = (!playerTurn ? cheminsImagesNoir : cheminsImagesBlanc);
    int promotionVers;

    int names[PIECES_PER_PLAYER] = {0, 'p', 'P', 't', 'f', 'r', 'R'};

    if (mouseX >= 680 && mouseX <= 680 + TAILLE_CASE) {
        promotionVers = TOUR;

    }
    else if (mouseX >= 680 + TAILLE_CASE && mouseX <= 680 + TAILLE_CASE * 2) {
        promotionVers = PONEY;

    }
    else if (mouseX >= 680 + TAILLE_CASE * 2 && mouseX <= 680 + TAILLE_CASE * 3) {
        promotionVers = FOU;

    }
    else if (mouseX >= 680 + TAILLE_CASE * 3 && mouseX <= 680 + TAILLE_CASE * 4) {
        promotionVers = REINE;
    }

    promotionDe(PIECE_NAME_PAWN, promotionVers, colorChemin);
    attendPromotion = false;

    char *temp = calloc(7, sizeof(char));

    strcpy(temp, moveMade);

    sprintf(moveMade, "%s+%c", temp, names[promotionVers]);

    saveMove = true;

    free(temp);
}

void promotionDe(Piece_st* PIECE_NAME_PAWN, namePiece_t promotionVers, const char **colorChemin) {
    PIECE_NAME_PAWN->name = promotionVers;
    PIECE_NAME_PAWN->texture = piecesTexture[(promotionVers + PIECE_NAME_PAWN->color * KING) - 1];
}

void faitCoupPredefinis(Board_t board, char *coupPredefinis[], int nCoup) {
    event_t event;
    IVec2_st posSrc, posDest; 
    Piece_st* pieceCible = NULL;

    namePiece_t promotionVers;

    moveSimulationRendering = true;

    renderFrame(board);

    for (int i = 0; i < nCoup; i++) {
        handleEvents(event, board);

        SDL_Delay(1000);

        playerTurn = !playerTurn;

        posSrc.x = coupPredefinis[i][0] - 'a';
        posSrc.y = BOARD_SIZE - (coupPredefinis[i][1] - '1') - 1;

        posDest.x = coupPredefinis[i][2] - 'a';
        posDest.y = BOARD_SIZE - (coupPredefinis[i][3] - '1') - 1;

        // debug("%s => (%d, %d)", coupPredefinis[i], posSrc.x, posSrc.y);

        pieceCible = board[posSrc.y][posSrc.x];

        if (!pieceCible) {
            error("%s:%d are wrong no piece here", coupPredefinis[i], i);
            return;
        }

        
        // mouvement de la 'pieceCible' à 'posDest' sur le 'board'
        mouvement(board, pieceCible, posDest);

        if (strlen(coupPredefinis[i]) == 6) {
            switch (coupPredefinis[i][5]) {
                case 't':
                    promotionVers = TOUR;
                    break;
                
                case 'p':
                    promotionVers = PONEY;
                    break;
                
                case 'f':
                    promotionVers = FOU;
                    break;

                case 'r':
                    promotionVers = REINE;
                    break;

                default:
                    break;
            }

            promotionDe(pieceCible, promotionVers, i % 2 ? cheminsImagesNoir : cheminsImagesBlanc);
            attendPromotion = false;
        }

        renderFrame(board);
    }

    moveSimulationRendering = false;
}