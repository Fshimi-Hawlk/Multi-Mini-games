#include "audio.h"

static bool audioInitialized = false;

Sound sound_jump;
Sound sound_doubleJump;
Sound sound_gameLaunch;

Sound meme;

void lobby_initAudio(void) {
    if (audioInitialized) return;

    InitAudioDevice();

    sound_jump = LoadSound(SOUNDS_PATH "lobby_jump.wav");
    sound_doubleJump = LoadSound(SOUNDS_PATH "lobby_jump.wav");
    SetSoundPitch(sound_doubleJump, 1.35);
    sound_gameLaunch = LoadSound(SOUNDS_PATH "lobby_gameLaunch.wav");

    meme = LoadSound(SOUNDS_PATH "meme.wav");

    audioInitialized = true;
}

void lobby_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_jump)) UnloadSound(sound_jump);
    if (IsSoundValid(sound_doubleJump)) UnloadSound(sound_doubleJump);
    if (IsSoundValid(sound_gameLaunch)) UnloadSound(sound_gameLaunch);

    if (IsSoundValid(meme)) UnloadSound(meme);


    CloseAudioDevice();
}