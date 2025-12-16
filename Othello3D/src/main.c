#include "setups/game.h"
#include "core/animation/animation.h"
#include "core/app/cameraController.h"
#include "core/game/board.h"
#include "rendering/board3D.h"
#include "ui/game.h"
#include "utils/globals.h"
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                               Implementation                               */
/* -------------------------------------------------------------------------- */

Font font;
const int fontSize = 24;
const float textHeightOffset = 1.0f;  // slight lift above board
const Color textColor = RED;

void drawCellCoords(void) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            Vector3 textPos = {
                x * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f,
                textHeightOffset,
                y * CELL_PX_SIZE + CELL_PX_SIZE / 2.0f
            };

            char coord[5] = {0};
            snprintf(coord, sizeof(coord), "%c%d", x + 'A', y + 1);

            Vector2 textSize = MeasureTextEx(font, coord, fontSize, 1.0f);
            Vector2 screenPos = GetWorldToScreenEx(textPos, camera, GetScreenWidth(), GetScreenHeight());

            // Draw in 2D screen space to ensure readability (simple and effective)
            DrawTextEx(font, coord, 
                        (Vector2){ screenPos.x - textSize.x / 2.0f, screenPos.y - textSize.y / 2.0f },
                        fontSize, 1.0f, textColor);
        }
    }

}

/**
 * @brief Main entry point.
 * @return Exit code.
 */
int main(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Othello");
    SetTargetFPS(60);

    font = GetFontDefault();

    int a = 34;
    int b = 35; 
    // XOR b ^ b = 0
    a ^= b; // a = a ^ b
    b ^= a; // b = b ^ (a ^ b) = a ^ b ^ b = a
    a ^= b; // a = (a ^ b) ^ a = b ^ a ^ a = b 

    Board_t board = {{0}};

    s64Vector2_St lastMove = {-1, -1};
    s64Vector2_St flippedPieces[BOARD_SIZE * BOARD_SIZE];
    u64_t         flippedCount = 0;

    initGame(board);

    while (!WindowShouldClose()) {
        s64Vector2_St clickedPos = updateCamera();

        if (animationsPlaying) advanceAnimation(board);

        gameEnded = processTurn(board, clickedPos, &lastMove, flippedPieces, &flippedCount);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        draw3DBoard(board, lastMove, flippedPieces, flippedCount);
        EndMode3D();
        
        drawUI();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}