/**
    @file ui/app.c
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-05
    @brief One clear sentence that tells what this file is actually for.

    Contributors:
        - Fshimi-Hawlk:
            - Provided documentation start-up
        - <Name>:
            - What you added / changed / fixed (keep it short)

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#include "ui/app.h"
#include "utils/globals.h"

void bingo_drawUI(const BingoGame_St* game) {
    // Timer during grace
    if (SHOW_DELAY <= game->currentCall.timer && game->currentCall.timer <= (SHOW_DELAY + GRACE_TIME)) {
        f32 fontSize = 32;
        char buf[16];
        sprintf(buf, "%.2f", GRACE_TIME * ((SHOW_DELAY + GRACE_TIME) - game->currentCall.timer));
        f32Vector2 textSize = MeasureTextEx(fonts[FONT48], buf, fontSize, 0);
        f32Vector2 textPos = {
            .x = game->layout.windowCenter.x - textSize.x / 2.0f,
            .y = game->layout.cardRect.y + game->layout.cardRect.height + fontSize * 1.5f,
        };

        DrawTextEx(fonts[FONT48], buf, textPos, fontSize, 0, BLACK);
    }

    // Fading ball call
    if (game->currentCall.timer <= SHOW_DELAY) {
        f32 fontSize = 48;
        
        f32Vector2 textSize = MeasureTextEx(fonts[FONT48], game->currentCall.displayedText, 0, fontSize);
        f32Vector2 textPos = {
            .x = game->layout.windowCenter.x - textSize.x / 4.0f,
            .y = game->layout.cardRect.y - fontSize * 1.5f,
        };

        DrawTextEx(
            fonts[FONT48],
            game->currentCall.displayedText,
            textPos, fontSize, 0,
            Fade(BLACK, 
                1.0f - (game->currentCall.timer / SHOW_DELAY)
            )
        );
    }
}