#include "utils/globals.h"
#include "core/game/board.h"

bool8 isInBound(const s8Vector2 pos) {
    return (0 <= pos.x) && (pos.x < game.board.width)
        && (0 <= pos.y) && (pos.y < game.board.height);
}