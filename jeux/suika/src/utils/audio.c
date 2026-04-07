#include "utils/audio.h"
#include "assetPath.h"
#include "logger.h"
#include <stdio.h>

static bool audioInitialized = false;
static char g_suikaSoundsPath[512] = {0};

Sound sound_drop;
Sound sound_merge;

static const char* suika_getSoundsPath(void) {
    if (g_suikaSoundsPath[0] != '\0') return g_suikaSoundsPath;
    static const char* candidates[] = {
        SOUNDS_PATH,                /* "jeux/suika/assets/sounds/" from project root */
        "jeux/suika/assets/sounds/",
        "assets/sounds/",
        NULL
    };
    findAssetBase("suika_drop.wav", candidates,
                  g_suikaSoundsPath, sizeof(g_suikaSoundsPath));
    return g_suikaSoundsPath;
}

void suika_initAudio(void) {
    if (audioInitialized) return;

    const char* sp = suika_getSoundsPath();
    char p[600];
    snprintf(p, sizeof(p), "%ssuika_drop.wav",  sp); sound_drop  = LoadSound(p);
    snprintf(p, sizeof(p), "%ssuika_merge.wav", sp); sound_merge = LoadSound(p);
    audioInitialized = true;
    log_info("Suika audio initialized (path: %s)", sp);
}

void suika_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_drop))  UnloadSound(sound_drop);
    if (IsSoundValid(sound_merge)) UnloadSound(sound_merge);
    audioInitialized = false;
}
