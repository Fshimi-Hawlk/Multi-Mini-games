#include "audio.h"

Sound sound_cardTurn;

void initAudio(void) {
    sound_cardTurn = LoadSound(SOUNDS_PATH "solitaire_cardTurn.wav");
}

void freeAudio(void) {
    UnloadSound(sound_cardTurn);
}