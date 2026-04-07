/**
    @file game.c (ui)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Main UI dispatch.
*/

#include "ui/game.h"
#include "ui/board.h"
#include "ui/shape.h"

#include "utils/globals.h"
#include "utils/utils.h"

f32Vector2 getTextCenterPosition(const char* const text, Font font, f32 fontSize, f32Vector2 pos) {
    f32Vector2 textSize = MeasureTextEx(font, text, fontSize, 1);
    return (f32Vector2) {
        .x = pos.x - textSize.x / 2.0f,
        .y = pos.y - textSize.y / 2.0f
    };
}

void drawText(const char *const text, Font font, f32 fontSize, f32Vector2 pos, Color tint) {
    pos = getTextCenterPosition(text, font, fontSize, pos);
    DrawTextEx(font, text, pos, fontSize, 1, tint);
}

void drawUI(const GameState_St* const game) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            static f32Vector2 scorePos = {
                .x = WINDOW_WIDTH * 1.5f / 10.0f,
                .y = WINDOW_HEIGHT * 2.0f / 10.0f
            };

            static f32Vector2 streakPos = {
                .x = WINDOW_WIDTH * 1.5f / 10.0f,
                .y = WINDOW_HEIGHT * 3.0f / 10.0f
            };
            
            static f32Vector2 gameOverPos = {
                .x = WINDOW_WIDTH * 8.5f / 10.0f,
                .y = WINDOW_HEIGHT * 3.0f / 10.0f
            };

            drawBoard(game->board);
            drawSlots(game->prefabManager.slots);

            drawText(game->scoring.scoreText, fonts[FONT48], 48, scorePos, WHITE);

            if (game->scoring.streakCount > 0) {
                Color streakTextColor = Fade(WHITE, game->scoring.streakGrace * 2.0f / game->scoring.streakCount);
                drawText(game->scoring.streakText, fonts[FONT48], 48, streakPos, streakTextColor);
            }

            if (mainGameState.gameOver) {
                drawText("Game Over", fonts[FONT48], 48, gameOverPos, RED);
            }
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                blockBlast_drawShape(shapeBag[i]);
            }
        } break;

        default: UNREACHABLE("SceneState_Et");
    }

}
