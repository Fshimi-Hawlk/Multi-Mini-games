/**
    @file rendering.c
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Rendering functions for chess.
*/
#include "rendering.h"
#include "globals.h"
#include "algo.h"

/**
    @brief Draw all pieces on the board.
*/
void drawPieces(void) {
    for (int i = 0; i < PIECES_PER_PLAYER; i++) {
        drawPiece(blackPlayer->pieces[i]);
        drawPiece(whitePlayer->pieces[i]);
    }
}

/**
    @brief Draw a single piece.
    @param[in] piece  The piece to draw
*/
void drawPiece(Piece_st* piece) {
    if (!piece->isTaken) {
        Texture2D pieceTexture = (piece->color == COLOR_PIECE_BLACK ? black_piece_textures : white_piece_textures)[piece->name];

        Rectangle src = {
            0, 0,
            (float)pieceTexture.width,
            (float)pieceTexture.height
        };

        Rectangle dst = {
            BOARD_OFFSET + piece->pos.x * CELL_PX_SIZE,
            BOARD_OFFSET + piece->pos.y * CELL_PX_SIZE,
            CELL_PX_SIZE,
            CELL_PX_SIZE
        };

        Vector2 origin = {0, 0};

        DrawTexturePro(pieceTexture, src, dst, origin, 0.0f, WHITE);
    }
}

/**
    @brief Draw a single cell.
    @param[in] x  The x coordinate of the cell
    @param[in] y  The y coordinate of the cell
    @param[in] c  The color to fill the cell with
*/
void drawCell(int x, int y, Color c) {
    int posX = BOARD_OFFSET + x * CELL_PX_SIZE;
    int posY = BOARD_OFFSET + y * CELL_PX_SIZE;

    DrawRectangle(posX, posY, CELL_PX_SIZE, CELL_PX_SIZE, c);
}

/**
    @brief Draw the checkerboard pattern.
*/
void drawCheckerboard(void) {
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            drawCell(i, j, ((i + j) % 2) ? DARK_COLOR : BRIGHT_COLOR);
}

/**
    @brief Draw the board border with coordinates.
*/
void drawBorder(void) {
    Font font = GetFontDefault();
    int thickness = 3;
    int fontSize = 20;

    Rectangle rect = {
        BOARD_OFFSET - thickness,
        BOARD_OFFSET - thickness,
        BOARD_SIZE * CELL_PX_SIZE + thickness * 2,
        BOARD_SIZE * CELL_PX_SIZE + thickness * 2
    };

    for (int i = 0; i < thickness; i++) {
        DrawRectangleLinesEx(rect, 1, BLACK);
        rect.x += 1;
        rect.y += 1;
        rect.width  -= 2;
        rect.height -= 2;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        char line[2];
        snprintf(line, sizeof(line), "%d", 8 - i);

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

    for (int i = 0; i < BOARD_SIZE; i++) {
        char col[2];
        snprintf(col, sizeof(col), "%c", 'a' + i);

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

/**
    @brief Draw possible move positions for selected piece.
    @param[in] board  The game board
*/
void drawPositionsPossibles(Board_t board) {
    IVec2_st pos;

    Piece_st* pieceTemp = NULL;

    for (int i = 0; i < nbPositionsPossibles; i++) {
        pos = positionsPossibles[i];
        
        if (!isInCheck(board, selectionnedPiece, pos.x, pos.y, playerTurn)) {
            int x = BOARD_OFFSET + pos.x * CELL_PX_SIZE;
            int y = BOARD_OFFSET + pos.y * CELL_PX_SIZE;
            
            pieceTemp = board[pos.y][pos.x];

            Texture2D* texture = pieceTemp ? &circleTexture : &dotTexture;
            
            Rectangle src = {
                0, 0,
                (float)texture->width,
                (float)texture->height
            };

            Rectangle dst = {
                x,
                y,
                CELL_PX_SIZE,
                CELL_PX_SIZE
            };

            Vector2 origin = {0, 0};

            DrawTexturePro(*texture, src, dst, origin, 0.0f, WHITE);
        }
    }
}

/**
    @brief Draw the pawn promotion menu.
*/
void printPromotion(void) {
    Player_st* joueur = !playerTurn ? blackPlayer : whitePlayer;
    Texture2D *textures = (joueur->color == COLOR_PIECE_BLACK) ? black_piece_textures : white_piece_textures;

    for (int i = 0; i < 4; i++) {
        int x = xPromotion + CELL_PX_SIZE * i;
        int y = 120;
        
        PieceName_et name = joueur->pieces[i]->name;

        Rectangle src = {
            0, 0,
            (float)textures[name].width,
            (float)textures[name].height
        };

        Rectangle dst = {
            x,
            y,
            CELL_PX_SIZE,
            CELL_PX_SIZE
        };

        Vector2 origin = {0, 0};

        DrawTexturePro(textures[name], src, dst, origin, 0.0f, WHITE);
    }
}

/**
    @brief Render a complete frame (clear, draw, display).
    @param[in] board  The game board
*/
void renderFrame(Board_t board) {
    ClearBackground(RAYWHITE);

    drawBorder();
    drawCheckerboard();

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

    drawPieces();

    const char *text = finished 
                     ? (patFinished ? "Draw" : (playerTurn ? "White victory" : "Black victory")) 
                     : (!playerTurn && !waitingForPromotion ? "White to move" : "Black to move");


    int fontSize = 25;
    int xText = xPrint + MeasureText(text, fontSize) / 2;
    DrawText(text, xText, yPrint, fontSize, BLACK);
}