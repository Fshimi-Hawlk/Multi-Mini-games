#include "setups/app.h"
#include "setups/audio.h"

#include "utils/common.h"

void lobby_initApp(void) {
    srand(time(NULL));

    SetTraceLogLevel(LOG_WARNING);

    InitWindow(systemSettings.video.width, systemSettings.video.height, systemSettings.video.title);
    SetTargetFPS(60);

    lobby_initAudio();
}