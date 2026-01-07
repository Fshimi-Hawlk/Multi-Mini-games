#include "ui/game.h"
#include "ui/board.h"
#include "ui/shape.h"

#include "utils/globals.h"
#include "utils/utils.h"

void drawUI(const GameState_St* const game) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            drawBoard(game->board);
            drawSlots(game->slots);

            f32Vector2 textSize = MeasureTextEx(appFont, game->scoreText, APP_TEXT_FONT_SIZE, 1);
            f32Vector2 textPos = {
                .x = (game->board.pos.x - textSize.x) / 2.0f,
                .y = game->board.pos.y + (BLOCK_PX_SIZE * game->board.height) / 3.0f - textSize.y / 2.0f
            };
            DrawTextEx(appFont, game->scoreText, textPos, APP_TEXT_FONT_SIZE, 1, WHITE);
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                drawShape(shapeBag[i]);
            }
        } break;

        default: UNREACHABLE("SceneState_Et");
    }

}
