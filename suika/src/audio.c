#include "audio.h"

Sound sound_drop;
Sound sound_merge;

void initAudio(void) {
    sound_drop = LoadSound(SOUNDS_PATH "suika_drop.wav");
    sound_merge = LoadSound(SOUNDS_PATH "suika_merge.wav");
}

void freeAudio(void) {
    UnloadSound(sound_drop);
    UnloadSound(sound_merge);
}