#include "audio.h"

static bool audioInitialized = false;

Sound sound_appleCrunch;

void snake_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_appleCrunch = LoadSound(SOUNDS_PATH "snake_appleCrunch.wav");

    audioInitialized = true;
}

void snake_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_appleCrunch)) UnloadSound(sound_appleCrunch);

    CloseAudioDevice();
}