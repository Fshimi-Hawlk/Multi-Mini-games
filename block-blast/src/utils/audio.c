#include "utils/audio.h"

static bool audioInitialized = false;

Sound sound_brickPlaced;
Sound sound_combo;
Sound sound_gameOver;
Sound sound_lineBreak;

void blockBlast_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_brickPlaced = LoadSound(SOUNDS_PATH "sound_brickPlaced.wav");
    sound_combo = LoadSound(SOUNDS_PATH "blockBlast_combo.wav");
    sound_gameOver = LoadSound(SOUNDS_PATH "blockBlast_gameOver.wav");
    sound_lineBreak = LoadSound(SOUNDS_PATH "blockBlast_lineBreak.wav");

    audioInitialized = true;
}

void blockBlast_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_brickPlaced)) UnloadSound(sound_brickPlaced);
    if (IsSoundValid(sound_combo)) UnloadSound(sound_combo);
    if (IsSoundValid(sound_gameOver)) UnloadSound(sound_gameOver);
    if (IsSoundValid(sound_lineBreak)) UnloadSound(sound_lineBreak);

    CloseAudioDevice();
}