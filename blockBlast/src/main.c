#include "utils/common.h"
#include "setups/app.h"

// typedef struct {
//     s8 board[UINT8_MAX];
//     u8 columnCount, rowCount;
// } Map_St;

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

int main(void) {
    initApp();

    while (!WindowShouldClose()) {
        BeginDrawing(); {
            ClearBackground(WHITE);
        } EndDrawing();
    }

    freeApp();

    return 0;
}