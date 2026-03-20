#include "utils/audio.h"

static bool audioInitialized = false;

Sound sound_lineClear;
Sound sound_shapePlaced;

void tetris_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_lineClear = LoadSound(SOUNDS_PATH "tetris_lineClear.wav");
    sound_shapePlaced = LoadSound(SOUNDS_PATH "tetris_shapePlaced.wav");

    audioInitialized = true;
}

void tetris_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_lineClear)) UnloadSound(sound_lineClear);
    if (IsSoundValid(sound_shapePlaced)) UnloadSound(sound_shapePlaced);

    CloseAudioDevice();
}