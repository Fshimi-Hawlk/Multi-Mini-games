#include "core/game/shape.h"
#include "raylib.h"
#include "ui/shape.h"
#include "utils/baseTypes.h"
#include "utils/common.h"
#include "utils/configs.h"
#include "utils/globals.h"
#include "utils/userTypes.h"

void drawShape(const ActivePrefab_St shape) {
    f32Vector2 center = getOffsetCenter(*shape.prefab);

    for (u8 i = 0; i < shape.prefab->blockCount; ++i) {
        f32 blockX = shape.pos.x + (f32) (shape.prefab->offsets[i].x - center.x) * BLOCK_PX_SIZE;
        f32 blockY = shape.pos.y + (f32) (center.y - shape.prefab->offsets[i].y) * BLOCK_PX_SIZE;

        u32 ix = roundf(blockX);
        u32 iy = roundf(blockY);

        DrawRectangle(ix, iy, BLOCK_PX_SIZE, BLOCK_PX_SIZE, blockColors[shape.colorIndex]);  // assume uniform color
        DrawRectangleLines(ix, iy, BLOCK_PX_SIZE, BLOCK_PX_SIZE, BLACK);
    }
}

void drawSlots(const PrefabSlots_t slots) {
    for (u8 i = 0; i < 3; ++i) {
        drawShape(slots[i]);
    }
}