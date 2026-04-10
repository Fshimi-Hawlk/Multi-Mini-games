#include "utils/audio.h"
#include "assetPath.h"
#include <stdio.h>

Sound sound_ballFall;
Sound sound_pinFall;
Sound sound_strike;

static char g_soundsPath[512] = {0};
static bool g_audioInitialized = false;

static const char* bowling_getSoundsPath(void) {
    if (g_soundsPath[0] != '\0') return g_soundsPath;
    static const char* candidates[] = {
        SOUNDS_PATH,
        "assets/sounds/",
        "jeux/bowling/assets/sounds/",
        "../jeux/bowling/assets/sounds/",
        NULL
    };
    findAssetBase("bowling_ballFall.wav", candidates, g_soundsPath, sizeof(g_soundsPath));
    return g_soundsPath;
}

void bowling_initAudio(void) {
    if (g_audioInitialized) return;  // Prevent double initialization
    if (!IsAudioDeviceReady()) InitAudioDevice();
    const char* sp = bowling_getSoundsPath();
    char p[600];
    snprintf(p, sizeof(p), "%sbowling_ballFall.wav", sp); sound_ballFall = LoadSound(p);
    snprintf(p, sizeof(p), "%sbowling_pinFall.wav",  sp); sound_pinFall  = LoadSound(p);
    snprintf(p, sizeof(p), "%sbowling_strike.wav",   sp); sound_strike   = LoadSound(p);
    g_audioInitialized = true;
}

void bowling_freeAudio(void) {
    if (!g_audioInitialized) return;  // Prevent double free
    UnloadSound(sound_ballFall);
    UnloadSound(sound_pinFall);
    UnloadSound(sound_strike);
    g_audioInitialized = false;
}
