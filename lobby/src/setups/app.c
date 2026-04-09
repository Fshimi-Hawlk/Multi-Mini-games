#include "setups/app.h"
#include "setups/audio.h"

#include "utils/globals.h"

void lobby_initApp(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    SetTargetFPS(60);

    lobby_initAudio();
    paramsMenu_init(&paramsMenu);
}

void lobby_freeApp(void) {
    lobby_freeAudio();

    CloseWindow();
}