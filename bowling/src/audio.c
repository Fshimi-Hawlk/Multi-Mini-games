#include "audio.h"
#include <stdio.h>
#include "logger.h"

static bool audioInitialized = false;

Sound sound_ballFall;
Sound sound_pinFall;
Sound sound_strike;

void initAudio(void) {
    if (audioInitialized) return;
    
    InitAudioDevice();
    sound_ballFall = LoadSound(SOUNDS_PATH "bowling_ballFall.wav");
    sound_pinFall = LoadSound(SOUNDS_PATH "bowling_pinFall.wav");
    sound_strike = LoadSound(SOUNDS_PATH "bowling_strike.wav");
    audioInitialized = true;
    log_info("Audio initialized successfully");
}

void freeAudio(void) {
    if (!audioInitialized) return;
    
    if (IsSoundValid(sound_ballFall)) UnloadSound(sound_ballFall);
    if (IsSoundValid(sound_pinFall)) UnloadSound(sound_pinFall);
    if (IsSoundValid(sound_strike)) UnloadSound(sound_strike);
    
    CloseAudioDevice();
    audioInitialized = false;
}
