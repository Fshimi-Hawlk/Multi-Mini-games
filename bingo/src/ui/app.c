/**
    @file app.c (ui)
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-19
    @brief One clear sentence that tells what this file is actually for.

    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "ui/app.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

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