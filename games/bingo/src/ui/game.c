/**
    @file game.c
    @author Kimi BERGE
    @date 2026-03-02
    @date 2026-04-14
    @brief Low-level drawing routines for Bingo gameplay elements.
*/
#include "ui/game.h"

#include "utils/globals.h"
#include "utils/userTypes.h"

/**
    @brief Draws the selection screen for choosing a Bingo card.

    @param[in]     layout       The layout containing choice card definitions.
*/
void bingo_drawChoiceCards(const Layout_St* layout) {
    const char* title = "Choose your bingo card";
    uint titleSize = 32;
    Vector2 textSize = MeasureTextEx(bingo_fonts[FONT32], title, titleSize, 0);
    Vector2 textPos = {
        layout->windowCenter.x - textSize.x / 2.0f,
        layout->choiceCards[0].innerRect.y - 60.0f
    };

    DrawTextEx(bingo_fonts[FONT32], title, textPos, titleSize, 0, BLACK);
    f32 roundness = 0.05f;

    for (uint idx = 0; idx < 12; ++idx) {
        const CardUI_St* card = &layout->choiceCards[idx];

        const Rectangle innerCard = card->innerRect;
        const Rectangle cardBackground = card->backgroundRect;
        const Rectangle border = card->backgroundRect;

        DrawRectangleRounded(cardBackground, roundness, 12, WHITE);
        DrawRectangleRoundedLinesEx(border, roundness - 0.025f, 12, 3.0f, card->selected ? GREEN : BROWN);

        // Squares + numbers
        f32 cellW = innerCard.width / 5.0f;

        for (u8 i = 0; i < 5; ++i) {
            Vector2 textSize = MeasureTextEx(bingo_fonts[FONT16], LETTERS[i], 14, 0);
            f32 lx = innerCard.x + cellW * (i + 0.5f) - textSize.x / 2.0f;
            f32 ly = innerCard.y + cellW / 7.5;
            DrawTextEx(bingo_fonts[FONT16], LETTERS[i], (Vector2) {lx, ly}, 14, 0, BLACK);
        }

        for (u8 row = 0; row < 5; ++row) {
            for (u8 col = 0; col < 5; ++col) {
                Rectangle cell = {
                    .x      = innerCard.x + col * cellW,
                    .y      = innerCard.y + 25 + row * cellW,
                    .width  = cellW - 1.0f,
                    .height = cellW - 1.0f
                };

                DrawRectangleLinesEx(cell, 1, GRAY);

                u8 textHeight = 12;
                const char* text = "FREE";

                if (row != 2 || col != 2) {
                    textHeight = 16;
                    text = TextFormat("%u", card->values[row][col]);
                }

                Vector2 textSize = MeasureTextEx(bingo_fonts[FONT32], text, textHeight, 0);

                DrawTextEx(
                    bingo_fonts[FONT32], text, 
                    (Vector2) {
                        .x = cell.x + (cell.width - textSize.x) / 2.0f, 
                        .y = cell.y + (cell.height - textSize.y) / 2.0f
                    }, textSize.y, 0, BLACK
                );
            }
        }
    }
}

/**
    @brief Draws the main player Bingo card.

    @param[in]     layout       The current game layout.
    @param[in]     player       The player's card and marking state.
*/
void bingo_drawCard(const Layout_St* const layout, const PlayerCard_St* const player) {
    f32 roundness = 0.05f;
    DrawRectangleRounded(layout->cardRect, roundness, 16, WHITE);
    DrawRectangleRoundedLinesEx(layout->cardRectBorder, roundness - 0.025f, 16, 6.5f, BROWN);

    // Column letters
    for (u8 i = 0; i < 5; ++i) {
        Vector2 textSize = MeasureTextEx(bingo_fonts[FONT32], LETTERS[i], 32, 0);
        Vector2 pos = {
            .x = layout->cardRectsRect.x + layout->cardRectsRect.width * (i + 0.5f) - textSize.x / 2.0f,
            .y = layout->cardRectsRect.y - layout->cardRectsRect.height / 1.5f
        };

        DrawTextEx(bingo_fonts[FONT32],LETTERS[i], pos, 32, 0, BLACK);
    }

    // Squares
    for (u8 r = 0; r < 5; ++r) {
        for (u8 c = 0; c < 5; ++c) {
            Rectangle rect = {
                .x      = layout->cardRectsRect.x + layout->cardRectsRect.width * c,
                .y      = layout->cardRectsRect.y + layout->cardRectsRect.height * r,
                .width  = layout->cardRectsRect.width - 1,
                .height = layout->cardRectsRect.height - 1
            };

            DrawRectangleLinesEx(rect, 1, BLACK);

            if (player->misclicks[r][c] > 0) {
                Rectangle innerRect = {
                    .x = rect.x + 1,
                    .y = rect.y + 1,
                    .width = rect.width - 2,
                    .height = rect.height - 2,
                };

                switch (player->misclicks[r][c]) {
                    case 1: DrawRectangleRec(innerRect, YELLOW); break;
                    case 2: DrawRectangleRec(innerRect, ORANGE); break;
                    case 3:
                    default: {
                        DrawRectangleRec(innerRect, RED);
                        
                        Vector2 textPos = {
                            .x = innerRect.x + layout->cardRectsRect.width / 2.0f - 10,
                            .y = innerRect.y + layout->cardRectsRect.height / 2.0f - 16, 
                        };

                        DrawTextEx(bingo_fonts[FONT32], "X", textPos, 32, 0, WHITE);
                    }
                }

                
            }

            if (player->daubs[r][c]) {
                DrawCircle(
                    rect.x + layout->cardRectsRect.width / 2.0f,
                    rect.y + layout->cardRectsRect.height / 2.0f,
                    layout->cardRectsRect.width / 2.0f - 5, 
                    GREEN
                );
            } else if (player->misclicks[r][c] < 3) {
                u32 textHeight = 24;
                const char *text = "Free";

                if (r != 2 || c != 2) {
                    textHeight = 32;
                    text = TextFormat("%u", player->numbers[r][c]);
                }

                Vector2 textSize = MeasureTextEx(bingo_fonts[FONT32], text, textHeight, 0);
                Vector2 textPos = {
                    .x = rect.x + (layout->cardRectsRect.width - textSize.x) / 2.0f,
                    .y = rect.y + (layout->cardRectsRect.height - textSize.y) / 2.0f,
                };
                DrawTextEx(bingo_fonts[FONT32], text, textPos, textSize.y, 0, BLACK);
            }
        }
    }
}
