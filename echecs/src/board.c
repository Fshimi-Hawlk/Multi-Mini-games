#include "board.h"
#include "global.h"

IVec2_st getBoardPosition() {
    Vector2 mouse = GetMousePosition();

    return (IVec2_st) {(mouse.x - BOARD_OFFSET) / CELL_PX_SIZE, (mouse.y - BOARD_OFFSET) / CELL_PX_SIZE};
}