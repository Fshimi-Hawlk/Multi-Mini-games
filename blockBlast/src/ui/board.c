/**
 * @file board.c (ui)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Board rendering implementation.
 */

#include "ui/board.h"
#include "utils/globals.h"

void drawBlock(const f32Vector2 pos, const color32 color) {
    s32 ix = roundf(pos.x);
    s32 iy = roundf(pos.y);

    DrawRectangle(ix, iy, BLOCK_PX_SIZE, BLOCK_PX_SIZE, color);
    DrawRectangleLines(ix, iy, BLOCK_PX_SIZE, BLOCK_PX_SIZE, BLOCK_OUTLINE_COLOR);
}

void drawBoard(const Board_St board) {
    for (u32 r = 0; r < board.height; ++r) {
        for (u32 c = 0; c < board.width; ++c) {
            const f32Vector2 tilePos = {
                .x = board.pos.x + BLOCK_PX_SIZE * c,
                .y = board.pos.y + BLOCK_PX_SIZE * r
            };
            
            Block_St block = board.blocks[r][c];
            color32 tileColor;
            if (block.hitsLeft == 0) {
                tileColor = BOARD_EMPTY_TILE_COLOR;
            } else if (block.hitsLeft < 0) {
                tileColor = APP_BACKGROUND_COLOR;
            } else {
                tileColor = blockColors[block.colorIndex];
                // Vector3 tileColor3 = ColorToHSV(tileColor);
                // tileColor = ColorBrightness(tileColor, tileColor3.z * (1 - ((block.hitsLeft - 1) / 10.0f)));
            }

            drawBlock(tilePos, tileColor);
        }
    }
}