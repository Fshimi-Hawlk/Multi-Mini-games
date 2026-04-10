#include "utils/audio.h"

#include "utils/globals.h"

static bool audioInitialized = false;

Sound sound_shapePlacement;
Sound sound_combo;
Sound sound_gameOver;
Sound sound_lineBreak;

void blockBlast_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_shapePlacement = LoadSound(SOUNDS_PATH "shapePlacement.wav");
    sound_combo = LoadSound(SOUNDS_PATH "combo.wav");
    sound_gameOver = LoadSound(SOUNDS_PATH "gameOver.wav");
    sound_lineBreak = LoadSound(SOUNDS_PATH "lineBreak.wav");

    audioInitialized = true;
}

void blockBlast_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_shapePlacement)) UnloadSound(sound_shapePlacement);
    if (IsSoundValid(sound_combo)) UnloadSound(sound_combo);
    if (IsSoundValid(sound_gameOver)) UnloadSound(sound_gameOver);
    if (IsSoundValid(sound_lineBreak)) UnloadSound(sound_lineBreak);

    CloseAudioDevice();
}