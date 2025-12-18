#include "rendering.h"
#include "global.h"

void drawPieces(Board_t board) {
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        afficherPiece(blackPlayer->pieces[i]);
        afficherPiece(whitePlayer->pieces[i]);
    }
}

void drawPiece(Piece_st* piece) {
    float pieceX = BOARD_OFFSET + piece->pos.x * CELL_PX_SIZE;
    float pieceY = BOARD_OFFSET + piece->pos.y * CELL_PX_SIZE;

    if (!piece->isTaken) {
        Texture2D pieceTexture = (piece->color == COLOR_PIECE_BLACK ? black_piece_textures : white_piece_textures)[piece->name];
        DrawTexture(pieceTexture, pieceX, pieceY, WHITE);
    }
}

void drawCell(int x, int y, Color c) {
    int posX = BOARD_OFFSET + x * CELL_PX_SIZE;
    int posY = BOARD_OFFSET + y * CELL_PX_SIZE;

    DrawRectangle(posX, posY, CELL_PX_SIZE, CELL_PX_SIZE, c);
}

void drawCheckerboard(void) {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            drawCell(i, j, ((i + j) % 2) ? DARK_COLOR : BRIGHT_COLOR);
}

void drawBorder(void) {
    Font font = GetFontDefault();
    int thickness = 3;
    int fontSize = 20;

    // Rectangle extérieur du board
    Rectangle rect = {
        BOARD_OFFSET - thickness,
        BOARD_OFFSET - thickness,
        BOARD_SIZE * CELL_PX_SIZE + thickness * 2,
        BOARD_SIZE * CELL_PX_SIZE + thickness * 2
    };

    // Dessiner un contour épais
    for (int i = 0; i < thickness; i++) {
        DrawRectangleLinesEx(rect, 1, BLACK);
        rect.x += 1;
        rect.y += 1;
        rect.width  -= 2;
        rect.height -= 2;
    }

    // Lignes 1–8 à gauche
    for (int i = 0; i < BOARD_SIZE; i++) {
        char line[2];
        sprintf(line, "%d", 8 - i);

        int fontSize = 20;
        Vector2 textSize = MeasureTextEx(font, line, fontSize, 1);

        DrawTextEx(
            font,
            line,
            (Vector2){
                BOARD_OFFSET - textSize.x - 5,
                BOARD_OFFSET + i * CELL_PX_SIZE + (CELL_PX_SIZE - textSize.y) / 2
            },
            fontSize,
            1,
            BLACK
        );
    }

    // Colonnes a–h en bas
    for (int i = 0; i < BOARD_SIZE; i++) {
        char col[2];
        sprintf(col, "%c", 'a' + i);

        Vector2 textSize = MeasureTextEx(font, col, fontSize, 1);

        DrawTextEx(
            font,
            col,
            (Vector2){
                BOARD_OFFSET + i * CELL_PX_SIZE + (CELL_PX_SIZE - textSize.x) / 2,
                BOARD_OFFSET + BOARD_SIZE * CELL_PX_SIZE
            },
            fontSize,
            1,
            BLACK
        );
    }
}

void drawPositionsPossibles(Board_t board) {
    IVec2_st pos;

    Piece_st* pieceTemp = NULL;

    for (int i = 0; i < nbPositionsPossibles; i++) {
        pos = positionsPossibles[i];

        if (!estEnEchec(board, pos.x, pos.y)) {
            int x = BOARD_OFFSET + pos.x * CELL_PX_SIZE;
            int y = BOARD_OFFSET + pos.y * CELL_PX_SIZE;

            pieceTemp = board[pos.y][pos.x];

            if (pieceTemp) {
                DrawTexture(circleTexture, x, y, WHITE);
            } 
            
            else {
                DrawTexture(dotTexture, x, y, WHITE);
            }
        }
    }
}

void printPromotion(void) {
    Player_st* joueur = !playerTurn ? blackPlayer : whitePlayer;
    Texture2D *textures = (joueur->color == COLOR_PIECE_BLACK) ? black_piece_textures : white_piece_textures;

    for (int i = 0; i < 4; i++) {
        int x = 680 + CELL_PX_SIZE * i ;
        int y = 120;

        PieceName_et name = joueur->pieces[i]->name;
        DrawTexture(textures[name], x, y, WHITE);
    }
}

void renderFrame(Board_t board) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    dessinerBordures();
    dessinerDamier();

    if (waitingForPromotion) {
        printPromotion();
    }

    if (previousMoveCell[0].x != -1) {
        drawCell(previousMoveCell[0].x, previousMoveCell[0].y, YELLOW_HIGHLIGHT);
        drawCell(previousMoveCell[1].x, previousMoveCell[1].y, YELLOW_HIGHLIGHT);
    }

    if (selectionnedPiece) {
        drawCell(selectionnedPiece->pos.x, selectionnedPiece->pos.y, RED_HIGHLIGHT);
        drawPositionsPossibles(board);
    }

    drawPieces(board);

    const char *text = finished ? (patFinished ? "Draw" : (playerTurn ? "White victory" : "Black victory")) : (!playerTurn && !waitingForPromotion ? "White to move" : "Black to move");
    DrawText(text, 20, 20, 24, BLACK);

    EndDrawing();
}