/**
 * @file game.c (ui)
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Main UI dispatch.
 */

#include "ui/game.h"
#include "ui/board.h"
#include "ui/shape.h"

#include "utils/globals.h"
#include "utils/utils.h"

f32Vector2 getTextCenterPosition(const char* const text, Font font, f32 fontSize, f32Vector2 pos) {
    f32Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);
    return (f32Vector2) {
        .x = (pos.x - textSize.x) / 2.0f,
        .y = (pos.y - textSize.y) / 2.0f
    };
}

void drawText(const char* const text, Font font, f32 fontSize, f32Vector2 pos, color32 tint) {
    DrawTextEx(
        font, 
        text, 
        getTextCenterPosition(
            text, 
            font, 
            fontSize, 
            pos
        ), 
        fontSize, 
        1, 
        tint
    );
}

void drawUI(const GameState_St* const game) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            drawBoard(game->board);
            drawSlots(game->prefabManager.slots);

            f32Vector2 scoreTextPos = {
                .x = game->board.pos.x,
                .y = game->board.pos.y + (BLOCK_PX_SIZE * game->board.height) / 3.0f
            };
            drawText(game->scoreText, fonts[FONT48], APP_TEXT_FONT_SIZE, scoreTextPos,WHITE);

            if (game->streakCount > 0) {
                f32Vector2 streakTextPos = {
                    .x = game->board.pos.x,
                    .y = game->board.pos.y + (BLOCK_PX_SIZE * game->board.height) * 2 / 3.0f
                };

                color32 streakTextColor = Fade(WHITE, 1 - (game->streakPlacementResetCnt * 2.0f / game->streakCount));
                drawText(game->streakText, fonts[FONT48], APP_TEXT_FONT_SIZE, streakTextPos, streakTextColor);
            }
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            for (u32 i = 0; i < game->prefabManager.prefabsBag.count; ++i) {
                drawShape(shapeBag[i]);
            }
        } break;

        default: UNREACHABLE("SceneState_Et");
    }

}
