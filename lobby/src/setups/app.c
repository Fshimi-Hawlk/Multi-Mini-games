#include "setups/app.h"
#include "setups/audio.h"

#include "utils/common.h"

void lobby_initApp(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Lobby Multi-Mini-Games");
    SetTargetFPS(60);

    lobby_initAudio();
}

void lobby_freeApp(void) {
    lobby_freeAudio();

    CloseWindow();
}