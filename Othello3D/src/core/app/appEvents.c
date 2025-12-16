#include "core/app/appEvents.h"
// #include "core/board/board.h"
// #include "core/game/gameEvents.h"
// #include "ui/button.h"
#include "utils/logger/logger.h"

// static void handleAppEvents(void) {
//     if (WindowShouldClose())
//         running = false;

//     updateButtons();
// }

// // Top-level event handler
// void handleEvents(BoardPtr_St board) {
//     handleAppEvents(); // Always handle app-wide events

//     switch (appState) {
//         case APP_STATE_MAIN_MENU:
//         case APP_STATE_GAME_STARTING_CONFIGURATION_SELECTION_MENU: {
//             if (showPane) {
//                 configPane.scrollOffset -= GetMouseWheelMoveV().y * SCROLL_SPEED;
//                 // Clamping handled in renderConfigButtonsInFrame
//             }
//         } break;

//         case APP_STATE_GAME_STARTING_CONFIGURATION_MAKING_MENU:
//             // updateButtons already called in handleAppEvents
//             break;
            
//         case APP_STATE_GAME_NORMAL:
//         case APP_STATE_GAME_CUSTOM:
//             handleGameEvents(board);
//             break;
//     }
// }