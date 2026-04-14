/**
    @file app.c
    @author Kimi BERGE
    @date 2026-03-02
    @date 2026-04-14
    @brief High-level UI drawing for the Bingo game.
*/
#include "ui/app.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

/**
    @brief Draws the top-level UI elements (timer and called ball text).

    @param[in]     layout       The current game layout.
    @param[in]     balls        The ball system state.
    @param[in]     currentCall  The current call state.
*/
void bingo_drawUI(const Layout_St* const layout, const BallSystem_St* const balls, const CallState_St* const currentCall) {
    // Timer during grace
    if (balls->showDelay <= currentCall->timer && currentCall->timer <= (balls->showDelay + balls->graceDelay)) {
        f32 fontSize = 32;
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f", balls->graceDelay * ((balls->showDelay + balls->graceDelay) - currentCall->timer));
        Vector2 textSize = MeasureTextEx(bingo_fonts[FONT48], buf, fontSize, 0);
        Vector2 textPos = {
            .x = layout->windowCenter.x - textSize.x / 2.0f,
            .y = layout->cardRect.y + layout->cardRect.height + fontSize * 1.5f,
        };

        DrawTextEx(bingo_fonts[FONT48], buf, textPos, fontSize, 0, BLACK);
    }

    // Fading ball call
    if (currentCall->timer <= balls->showDelay) {
        f32 fontSize = 48;
        
        Vector2 textSize = MeasureTextEx(bingo_fonts[FONT48], currentCall->displayedText, fontSize, 0);
        Vector2 textPos = {
            .x = layout->windowCenter.x - textSize.x / 4.0f,
            .y = layout->cardRect.y - fontSize * 1.5f,
        };

        DrawTextEx(
            bingo_fonts[FONT48],
            currentCall->displayedText,
            textPos, fontSize, 0,
            Fade(BLACK, 
                1.0f - (currentCall->timer / balls->showDelay)
            )
        );
    }
}