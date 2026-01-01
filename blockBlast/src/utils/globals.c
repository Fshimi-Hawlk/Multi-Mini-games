#include "utils/globals.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[MAX_FONT_COUNT] = {0};

const f32Vector2 defaultPositions[3] = {
    {.x = WINDOW_WIDTH * 20 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
    {.x = WINDOW_WIDTH * 50 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
    {.x = WINDOW_WIDTH * 80 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
};

const color32 cellColors[_cellColorCount] = {
    { .r = 254, .g =  40, .b =  44, .a = 225 }, // Red
    { .r = 232, .g = 116, .b =  33, .a = 225 }, // Orange
    { .r = 242, .g = 187, .b =  58, .a = 255 }, // Yellow
    { .r =  58, .g = 175, .b =  68, .a = 255 }, // Green
    { .r =  46, .g = 215, .b = 255, .a = 225 }, // Skyblue
    { .r =  74, .g =  99, .b = 226, .a = 255 }, // Blue
    { .r = 135, .g =  91, .b = 206, .a = 255 }, // Purple
    { .r = 255, .b = 109, .g = 194, .a = 255 }, // Pink
};

const PrefabData_St prefabs[] = {
    { .cellCount = 1, .DOC = 0, .canMirror = false, .relPos = { {0, 0} } }, // unit (1x1)
    { .cellCount = 2, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1} } }, // Vertical (1x2)
    { .cellCount = 3, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2} } }, // Vertical (1x3)
    { .cellCount = 3, .DOC = 4, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {1, 0} } }, // L-shape
    { .cellCount = 4, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {0, 3} } }, // Vertical (1x4)
    { .cellCount = 4, .DOC = 0, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {1, 0}, {1, 1} } }, // 2x2-shape
    { .cellCount = 4, .DOC = 4, .canMirror = false, .relPos = { {0, 0}, {1, 0}, {2, 0}, {1, 1} } }, // T-shape
    { .cellCount = 4, .DOC = 4, .canMirror = true,  .relPos = { {0, 0}, {1, 0}, {0, 1}, {0, 2} } }, // L-shape
    { .cellCount = 4, .DOC = 2, .canMirror = true,  .relPos = { {0, 0}, {0, 1}, {1, 1}, {1, 2} } }, // Z-shape
    { .cellCount = 5, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4} } }, // Vertical (1x5)
    { .cellCount = 5, .DOC = 0, .canMirror = false, .relPos = { {0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1} } }, // Plus-shape
    { .cellCount = 5, .DOC = 4, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0} } }, // L-shape
    { .cellCount = 6, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5} } }, // Vertical (1x6)
    { .cellCount = 6, .DOC = 2, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2} } }, // 2x3 rectangle
    { .cellCount = 9, .DOC = 0, .canMirror = false, .relPos = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2} } }, // 2x3 rectangle
};

const u32 prefabCount = sizeof(prefabs) / sizeof(*prefabs);
