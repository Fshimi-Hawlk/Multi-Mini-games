#include "utils/audio.h"

static bool audioInitialized = false;

Sound sound_check;
Sound sound_checkMate;
Sound sound_move;
Sound sound_promotion;

void chess_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_check = LoadSound(SOUNDS_PATH "chess_check.wav");
    sound_checkMate = LoadSound(SOUNDS_PATH "chess_checkMate.wav");
    sound_move = LoadSound(SOUNDS_PATH "chess_move.wav");
    sound_promotion = LoadSound(SOUNDS_PATH "chess_promotion.wav");

    audioInitialized = true;
}

void chess_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_check)) UnloadSound(sound_check);
    if (IsSoundValid(sound_checkMate)) UnloadSound(sound_checkMate);
    if (IsSoundValid(sound_move)) UnloadSound(sound_move);
    if (IsSoundValid(sound_promotion)) UnloadSound(sound_promotion);

    CloseAudioDevice();
}
