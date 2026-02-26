/**
    @file shape.c (ui)
    @author Fshimi Hawlk
    @date 2026-01-07
    @brief Rendering of shapes and slots.
*/

#include "core/shape.h"
#include "ui/shape.h"
#include "ui/board.h"
// #include "utils/configs.h"
#include "utils/globals.h"

void drawShape(const Shape_St shape) {
    for (u8 i = 0; i < shape.prefab->blockCount; ++i) {
        drawBlock(getIthBlockPosition(shape, i), blockColors[shape.colorIndex]);
    }
}

void drawSlots(const ShapeSlots_t slots) {
    for (u8 i = 0; i < 3; ++i) {
        if (slots[i].placed) continue;
        drawShape(slots[i]);
    }
}