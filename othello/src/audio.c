#include "audio.h"

static bool audioInitialized = false;

Sound sound_piecePutDown;

void othello_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_piecePutDown = LoadSound(SOUNDS_PATH "othello_piecePutDown.wav");

    audioInitialized = true;
}

void othello_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_piecePutDown)) UnloadSound(sound_piecePutDown);

    CloseAudioDevice();
}