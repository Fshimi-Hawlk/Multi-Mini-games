#include "rendering/board3D.h"
#include "core/game/boardUtils.h"
#include "utils/globals.h"
#include "utils/config.h"

#include <rlgl.h>

void draw3DBoard(const Board_t board, s64Vector2_St lastMove, const s64Vector2_St flippedPieces[], u64_t flippedCount) {
    DrawPlane(BOARD_CENTER, (Vector2){BOARD_PX_SIZE, BOARD_PX_SIZE}, DARKGREEN);

    for (int i = 0; i <= BOARD_SIZE; i++) {
        float p = i * CELL_PX_SIZE;
        DrawLine3D((Vector3) {p, 0.1f, 0}, (Vector3) {p, 0.1f, BOARD_SIZE*CELL_PX_SIZE}, BLACK);
        DrawLine3D((Vector3) {0, 0.1f, p}, (Vector3) {BOARD_SIZE*CELL_PX_SIZE, 0.1f, p}, BLACK);
    }

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            bool isAnimating = false;
            for (u64_t a = 0; a < animCount; a++) {
                if (animPieces[a].pos.x == x && animPieces[a].pos.y == y) {
                    isAnimating = true;
                    break;
                }
            }

            if (isAnimating) continue;

            if (board[y][x] == EMPTY) continue;
            Vector3 pos = {x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 8.0f, y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};
            Color col = (board[y][x] == PLAYER) ? BLACK : WHITE;
            Color antiCol = (board[y][x] == PLAYER) ? WHITE : BLACK;
            DrawCylinderEx(pos, Vector3Add(pos, (Vector3){0,5,0}), CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, col);
            DrawCylinderEx(Vector3Add(pos, (Vector3){0,5,0}), Vector3Add(pos, (Vector3){0,10,0}), CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, antiCol);
        }
    }

    for (u64_t a = 0; a < animCount; a++) {
        AnimPiece_St* anim = &animPieces[a];
        float t = anim->timer / anim->duration;  // 0 to 1

        Vector3 basePos = {
            anim->pos.x * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f,
            8.0f,
            anim->pos.y * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f
        };

        Color antiCol = (anim->toState == PLAYER) ? WHITE : BLACK;
        Color col = (anim->toState == PLAYER) ? BLACK : WHITE;
        float heightOffset = 0.0f;

        if (anim->type == ANIM_PLACE) {
            // Fall from high
            float startHeight = 100.0f;
            basePos.y = startHeight * (1.0f - t) + 8.0f * t;
        } else if (anim->type == ANIM_FLIP) {
            // Arc height: peak at t=0.5
            const float maxArcHeight = 30.0f;
            heightOffset = maxArcHeight * sinf(PI * t);

            // Rotation 180° around X axis
            float angleDeg = 180.0f * t;

            rlPushMatrix();
            rlTranslatef(basePos.x, basePos.y + heightOffset + 5.0f, basePos.z);  // move to center
            rlRotatef(angleDeg, 1.0f, 0.0f, 0.0f);  // rotate around X
            rlTranslatef(0.0f, -5.0f, 0.0f);  // back to base

            DrawCylinderEx((Vector3) {0, 0, 0}, (Vector3) {0, 5, 0}, CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, antiCol);
            DrawCylinderEx((Vector3) {0, 5, 0}, (Vector3) {0, 10, 0}, CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, col);
            rlPopMatrix();

            continue;  // skip normal draw below
        } else {
            basePos.y += heightOffset;
        }

        // Normal draw for place (or fallback)
        DrawCylinderEx(basePos, Vector3Add(basePos, (Vector3){0,5,0}), CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, col);
        DrawCylinderEx(Vector3Add(basePos, (Vector3){0,5,0}), Vector3Add(basePos, (Vector3){0,10,0}), CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, antiCol);
    }

    // Highlights: only after all animations are done
    if (!animationsPlaying && lastMove.x >= 0) {
        Vector3 lmPos = {lastMove.x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 8.0f, lastMove.y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};
        DrawCylinderEx(lmPos, Vector3Add(lmPos, (Vector3){0,2,0}), CELL_PX_SIZE*0.42f, CELL_PX_SIZE*0.42f, 16, RED);

        for (u64_t i = 0; i < flippedCount; i++) {
            Vector3 fpPos = {flippedPieces[i].x * CELL_PX_SIZE + CELL_PX_SIZE/2.0f, 8.0f, flippedPieces[i].y * CELL_PX_SIZE + CELL_PX_SIZE/2.0f};
            DrawCylinderEx(fpPos, Vector3Add(fpPos, (Vector3){0,2,0}), CELL_PX_SIZE*0.42f, CELL_PX_SIZE*0.42f, 16, ORANGE);
        }
    }
    
    // Ghost piece
    if (cursorVisible && isPlayerTurn && hoveredCell.x != -1) {
        bool isValid = isValidMove(board, hoveredCell, true);
        Color ghostCol = isValid ? GREEN : RED;
        ghostCol.a = 128;

        Vector3 ghostPos = { hoveredCell.x * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f, 50.0f, hoveredCell.y * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f };

        DrawCylinderEx(ghostPos, Vector3Add(ghostPos, (Vector3){0,10,0}), CELL_PX_SIZE*0.4f, CELL_PX_SIZE*0.4f, 16, ghostCol);
    }
}