/**
    @file shape.c
    @author Fshimi-Hawlk
    @date 2026-01-07
    @date 2026-04-14
    @brief Rendering of shapes and slots.
*/
#include "core/shape.h"
#include "ui/shape.h"
#include "ui/board.h"
#include "utils/globals.h"

void polyBlast_drawShape(const Shape_St shape) {
    for (u8 i = 0; i < shape.prefab->blockCount; ++i) {
        polyBlast_drawBlock(polyBlast_getIthBlockPosition(shape, i), polyBlast_blockColors[shape.colorIndex]);
    }
}

void polyBlast_drawSlots(const ShapeSlots_t slots) {
    for (u8 i = 0; i < 3; ++i) {
        if (slots[i].placed) continue;
        polyBlast_drawShape(slots[i]);
    }
}