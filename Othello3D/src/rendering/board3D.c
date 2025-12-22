#include "rendering/board3D.h"
#include "core/game/boardUtils.h"
#include "utils/common.h"
#include "utils/globals.h"
#include "utils/logger/logger.h"
#include "utils/types.h"

void drawPiece(Vector3 basePos, f32_t angleDeg, Color topColor, Color bottomColor) {
    rlPushMatrix();
    rlTranslatef(basePos.x, basePos.y, basePos.z);
    rlRotatef(angleDeg, 1.0f, 0.0f, 0.0f);
    
    // Top half
    DrawModel(pieceModel, (Vector3) {0,8.0f,0}, 25.0f, bottomColor);
    
    // Bottom half
    rlRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    DrawModel(pieceModel, (Vector3) {0,-8.0f,0}, 25.0f, topColor);
    rlPopMatrix();
}

void draw3DBoard(const Board_t board, s64Vector2_St lastMove, const s64Vector2_St flippedPieces[], u64_t flippedCount) {

    static Vector3 boardPos = {
        .x = CELL_PX_SIZE * 4.0f,
        .y = 8,
        .z = CELL_PX_SIZE * 4.0f
    };

    DrawModel(boardModel, boardPos, 7.5f, WHITE);

    // Pieces material: plastic shine
    float pieceShininess = 20.0f;
    SetShaderValue(lightingShader, locShininess, &pieceShininess, SHADER_UNIFORM_FLOAT);

    float pieceSpecular = 0.6f;
    if (locSpecular != -1) SetShaderValue(lightingShader, locSpecular, &pieceSpecular, SHADER_UNIFORM_FLOAT);

    // Static pieces
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            bool isAnimating = false;
            for (u64_t a = 0; a < animCount; a++) {
                if (animPieces[a].pos.x == x && animPieces[a].pos.y == y) {
                    isAnimating = true;
                    break;
                }
            }

            if (isAnimating || board[y][x] == EMPTY) continue;

            Vector3 basePos = {x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 0.0f, y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};

            Color bottomColor = (board[y][x] == PLAYER) ? BLACK : WHITE;
            Color topColor = (board[y][x] == PLAYER) ? WHITE : BLACK;

            drawPiece(basePos, 0.0f, topColor, bottomColor);
        }
    }

    // Animated pieces
    for (u64_t a = 0; a < animCount; a++) {
        AnimPiece_St* anim = &animPieces[a];
        float t = anim->timer / anim->duration;

        Vector3 basePos = {
            anim->pos.x * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f,
            8.0f,
            anim->pos.y * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f
        };

        float heightOffset = 0.0f;
        float angleDeg = 0.0f;

        if (anim->type == ANIM_PLACE) {
            float startHeight = 50.0f;
            heightOffset = startHeight * (1.0f - t) + basePos.y * t;
            angleDeg = 180.0f;
        } else if (anim->type == ANIM_FLIP) {
            const float maxArcHeight = 50.0f;
            heightOffset = maxArcHeight * sinf(PI * t);
            angleDeg = 180.0f * t;
        }

        Color bottomColor = (anim->toState == PLAYER) ? WHITE : BLACK;
        Color topColor = (anim->toState == PLAYER) ? BLACK : WHITE;
        
        basePos.y += heightOffset;
        drawPiece(basePos, angleDeg, topColor, bottomColor);
    }

    // Highlights (use same pieceModel scaled thin for ring)
    if (!animationsPlaying && lastMove.x >= 0) {
        Vector3 lmPos = {lastMove.x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 2.0f, lastMove.y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};
        rlPushMatrix();
        rlTranslatef(lmPos.x, lmPos.y, lmPos.z);
        rlScalef(1.0f, 0.2f, 1.0f);  // flatten to ring
        DrawModel(ringModel, (Vector3) {0,0,0}, 1.0f, RED);
        rlPopMatrix();

        for (u64_t i = 0; i < flippedCount; i++) {
            Vector3 fpPos = {flippedPieces[i].x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 2.0f, flippedPieces[i].y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};
            rlPushMatrix();
            rlTranslatef(fpPos.x, fpPos.y, fpPos.z);
            rlScalef(1.0f, 0.2f, 1.0f);
            DrawModel(ringModel, (Vector3){0,0,0}, 1.0f, ORANGE);
            rlPopMatrix();
        }
    }

    // Ghost piece
    if (!animationsPlaying && cursorVisible && isPlayerTurn && hoveredCell.x != -1) {
        bool isValid = isValidMove(board, hoveredCell, true);
        Color ghostCol = isValid ? GREEN : RED;

        Vector3 ghostPos = { hoveredCell.x * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f, 50.0f, hoveredCell.y * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f };
        drawPiece(ghostPos, 0.0f, WHITE, BLACK);

        rlPushMatrix();
        rlTranslatef(ghostPos.x, ghostPos.y, ghostPos.z);
        rlScalef(1.0f, 0.2f, 1.0f);
        DrawModel(ringModel, (Vector3) {0,0,0}, 1.05f, ghostCol);
        rlPopMatrix();
    }
}