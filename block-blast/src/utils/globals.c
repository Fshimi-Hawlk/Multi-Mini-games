/**
 * @file globals.c
 * @author Fshimi Hawlk
 * @date 2026-01-07
 * @brief Definitions of global variables.
 */

#include "utils/globals.h"
#include "utils/userTypes.h"

Rectangle windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
Font      fonts[_fontSizeCount] = {0};
Font      appFont = {0};

PrefabBag_St prefabsBag = {0};
PrefabIndexBag_St bags[MAX_SHAPE_SIZE] = {0};
ActivePrefab_St *shapeBag = NULL;

u32 prefabsPerSizeOffsets[MAX_SHAPE_SIZE] = {0};

GameState_St game = {0};
GameState_St algoGame = {0};

f32Vector2 mouseDeltaFromShapeCenter = {0};
bool8 dragging = false;

const f32Vector2 defaultPositions[3] = {
    {.x = WINDOW_WIDTH * 17.5 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
    {.x = WINDOW_WIDTH * 50 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
    {.x = WINDOW_WIDTH * 82.5 / 100.0f, .y = WINDOW_HEIGHT * 85 / 100.0f},
};

const color32 blockColors[_blockColorCount] = {
    { .r = 254, .g =  40, .b =  44, .a = 255 }, // Red
    { .r = 232, .g = 116, .b =  33, .a = 255 }, // Orange
    { .r = 242, .g = 187, .b =  58, .a = 255 }, // Yellow
    { .r =  58, .g = 175, .b =  68, .a = 255 }, // Green
    { .r =  46, .g = 215, .b = 255, .a = 255 }, // Skyblue
    { .r =  74, .g =  99, .b = 226, .a = 255 }, // Blue
    { .r = 135, .g =  91, .b = 206, .a = 255 }, // Purple
    { .r = 255, .b = 109, .g = 194, .a = 255 }, // Pink
};

const Prefab_St prefabs[] = {
    [PREFAB_1x1]  = { .blockCount = 1, .orientations = -1, .offsets = { {0, 0} } },
    [PREFAB_2x2]  = { .blockCount = 4, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {1, 0}, {1, 1} } },
    [PREFAB_2x3]  = { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2} } },
    [PREFAB_3x3]  = { .blockCount = 9, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}, {2, 0}, {2, 1}, {2, 2} } },
    [PREFAB_1x2]  = { .blockCount = 2, .orientations = -1, .offsets = { {0, 0}, {0, 1} } },
    [PREFAB_1x3]  = { .blockCount = 3, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2} } },
    [PREFAB_1x4]  = { .blockCount = 4, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3} } },
    [PREFAB_1x5]  = { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4} } },
    [PREFAB_1x6]  = { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5} } },
    [PREFAB_L3]   = { .blockCount = 3, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {1, 0} } },
    [PREFAB_L4]   = { .blockCount = 4, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {0, 1}, {0, 2} } },
    [PREFAB_L5]   = { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0} } },
    [PREFAB_T]    = { .blockCount = 4, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {2, 0}, {1, 1} } },
    [PREFAB_Z]    = { .blockCount = 4, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {1, 1}, {1, 2} } },
    [PREFAB_PLUS] = { .blockCount = 5, .orientations = -1, .offsets = { {0, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 1} } },
    [_prefabNameCount] = { .blockCount = 5, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 1}, {1, 2} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {1, 1}, {1, 2}, {1, 3} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {1, 1}, {0, 2}, {1, 2} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 2} } },
    { .blockCount = 5, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 0} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {2, 0} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 1}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 1}, {2, 0} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 1}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 1}, {1, 2}, {0, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 1}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 1}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 0} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {0, 2}, {0, 3}, {1, 1}, {1, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {1, 1}, {1, 2}, {0, 2}, {0, 3}, {2, 0} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {1, 1}, {1, 2}, {0, 2}, {0, 3}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {2, 0}, {1, 1}, {1, 2}, {0, 2}, {0, 3}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {2, 0}, {1, 1}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {1, 1}, {0, 2}, {0, 3}, {1, 2}, {2, 2} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 0}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {2, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {3, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 1}, {3, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {1, 0}, {2, 0}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 3} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {0, 1}, {0, 2}, {1, 1}, {1, 2}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {0, 0}, {1, 0}, {0, 2}, {1, 1}, {1, 2}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {1, 1}, {0, 2}, {1, 2}, {1, 3}, {2, 1} } },
    { .blockCount = 6, .orientations = -1, .offsets = { {1, 0}, {1, 1}, {0, 2}, {1, 2}, {1, 3}, {2, 2} } },
};

const u32 prefabCount = sizeof(prefabs) / sizeof(*prefabs);


// Map_St maps[] = {
//     {
//         .board = {0},
//         .columnCount = 8,
//         .rowCount = 8,
//     },
//     {
//         .board = {
//             -1, -1,  0,  0,  0,  0,  0,  0, -1, -1,
//             -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
//              0,  0,  1,  0, -1, -1,  0,  1,  0,  0,
//              0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//              0,  0, -1,  0,  0,  0,  0, -1,  0,  0,
//              0,  0, -1,  0,  0,  0,  0, -1,  0,  0,
//              0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//              0,  0,  1,  0, -1, -1,  0,  1,  0,  0,
//             -1,  0,  0,  0,  0,  0,  0,  0,  0, -1,
//             -1, -1,  0,  0,  0,  0,  0,  0, -1, -1,
//         },
//         .columnCount = 10,
//         .rowCount = 10,
//     },
// };