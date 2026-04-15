/**
    @file audio.c
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Audio loading and management for the Bowling mini-game.
*/
#include "utils/audio.h"
#include "assetPath.h"
#include <stdio.h>

/** @brief Sound played when the ball falls on the lane. */
Sound sound_ballFall;
/** @brief Sound played when a pin falls or is hit. */
Sound sound_pinFall;
/** @brief Sound played when a strike or major collision occurs. */
Sound sound_strike;

/** @brief Cached path to the sounds directory. */
static char g_soundsPath[512] = {0};
/** @brief Flag indicating if audio has been initialized. */
static bool g_audioInitialized = false;

/**
    @brief Resolves and returns the path to the sounds directory.
    @return Pointer to the sounds path string
*/
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

/**
    @brief Initializes the audio device and loads bowling sound effects.
*/
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

/**
    @brief Unloads bowling sound effects.
*/
void bowling_freeAudio(void) {
    if (!g_audioInitialized) return;  // Prevent double free
    UnloadSound(sound_ballFall);
    UnloadSound(sound_pinFall);
    UnloadSound(sound_strike);
    g_audioInitialized = false;
}
