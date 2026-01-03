#include "ui/game.h"
#include "ui/board.h"
#include "ui/shape.h"
#include "utils/globals.h"

void drawUI(const GameState_St* const game) {
    switch (game->sceneState) {
        case SCENE_STATE_GAME: {
            drawBoard(game->board);
            drawSlots(game->slots);
        } break;

        case SCENE_STATE_ALL_PREFABS: {
            for (u32 i = 0; i < prefabsBag.count; ++i) {
                drawShape(shapeBag[i]);
            }
        } break;

        default: UNREACHABLE("SceneState_Et");
    }

}
