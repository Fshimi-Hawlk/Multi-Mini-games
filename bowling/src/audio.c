#include "audio.h"

Sound sound_ballFall;
Sound sound_pinFall;
Sound sound_strike;

void initAudio(void) {
    sound_ballFall = LoadSound(SOUNDS_PATH "bowling_ballFall.wav");
    sound_pinFall = LoadSound(SOUNDS_PATH "bowling_pinFall.wav");
    sound_strike = LoadSound(SOUNDS_PATH "bowling_strike.wav");
}

void freeAudio(void) {
    UnloadSound(sound_ballFall);
    UnloadSound(sound_pinFall);
    UnloadSound(sound_strike);
}
