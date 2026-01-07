#include "core/game/shape.h"
#include "ui/shape.h"
#include "ui/board.h"
// #include "utils/configs.h"
#include "utils/globals.h"

void drawShape(const ActivePrefab_St shape) {
    for (u8 i = 0; i < shape.prefab->blockCount; ++i) {
        drawBlock(getIthBlockPosition(shape, i), blockColors[shape.colorIndex]);
    }
}

void drawSlots(const PrefabSlots_t slots) {
    for (u8 i = 0; i < 3; ++i) {
        if (slots[i].placed) continue;
        drawShape(slots[i]);

        // f32Vector2 deOffsettedShapePos = Vector2Subtract(slots[i].center, (f32Vector2) {BLOCK_PX_SIZE * (slots[i].prefab->width - 1) / 2.0f, BLOCK_PX_SIZE * (slots[i].prefab->height - 1) / 2.0f});

        // f32Vector2 shapePos = Vector2AddValue(getShapeTopLeftCorner(&slots[i]), BLOCK_PX_SIZE / 2.0f);

        // DrawCircleV(slots[i].center, 5, RED);
        // DrawCircleV(shapePos, 5, ORANGE);
    }
}