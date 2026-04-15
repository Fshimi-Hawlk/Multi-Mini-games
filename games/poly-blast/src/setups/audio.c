/**
    @file audio.c
    @author Fshimi-Hawlk
    @date 2026-04-10
    @date 2026-04-14
    @brief Implementation of audio resource loading and unloading for game sound effects.
*/
#include "setups/audio.h"

#include "utils/globals.h"

static bool audioInitialized = false;

Sound sound_shapePlacement;
Sound sound_combo;
Sound sound_gameOver;
Sound sound_lineBreak;

void polyBlast_initAudio(void) {
    if (audioInitialized) return;

    sound_shapePlacement = LoadSound(SOUNDS_PATH "shapePlacement.wav");
    sound_combo = LoadSound(SOUNDS_PATH "combo.wav");
    sound_gameOver = LoadSound(SOUNDS_PATH "gameOver.wav");
    sound_lineBreak = LoadSound(SOUNDS_PATH "lineBreak.wav");

    audioInitialized = true;
}

void polyBlast_freeAudio(void) {
    if (!audioInitialized) return;
    audioInitialized = false;

    if (IsSoundValid(sound_shapePlacement)) UnloadSound(sound_shapePlacement);
    if (IsSoundValid(sound_combo)) UnloadSound(sound_combo);
    if (IsSoundValid(sound_gameOver)) UnloadSound(sound_gameOver);
    if (IsSoundValid(sound_lineBreak)) UnloadSound(sound_lineBreak);
}