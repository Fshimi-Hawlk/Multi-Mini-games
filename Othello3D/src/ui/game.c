#include "ui/game.h"
#include "raylib.h"
#include "utils/config.h"
#include "utils/globals.h"
#include "utils/types.h"
// #include "utils/logger/logger.h"

void drawUI(void) {
    Rectangle toggleBtn = { (float)GetScreenWidth() - 170, (float)GetScreenHeight() - 70, 160, 50 };
    const char* btnText = isTopDownView ? "Normal" : "FreeCam";

    DrawRectangleRec(toggleBtn, Fade(LIGHTGRAY, 0.8f));
    DrawRectangleLinesEx(toggleBtn, 3, BLACK);
    DrawText(btnText, toggleBtn.x + 12, toggleBtn.y + 15, 24, BLACK);

    DrawText(TextFormat("Player: %llu  |  Computer: %llu", playerScore, computerScore),
             20, GetScreenHeight() - 40, 28, BLACK);

    if (gameEnded) {
        const char *winner = (playerScore > computerScore) ? "You" : "Computer";
        Color textColor    = (playerScore > computerScore) ? BLUE : RED;

        f32_t endTextFontSize = 40;
        const char *endText = TextFormat("%s won!", winner);
        Vector2 textSize = MeasureTextEx(GetFontDefault(), endText, endTextFontSize, 0);

        DrawText(endText, WINDOW_WIDTH - textSize.x * 1.5f, (WINDOW_HEIGHT - textSize.y) / 2.0f, endTextFontSize, textColor);
    }
}
