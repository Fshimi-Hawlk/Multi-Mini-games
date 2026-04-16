/**
    @file audio.c
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief audio.c implementation/header file
*/
#include "setups/audio.h"
#include "utils/globals.h"

static bool audioInitialized = false;

void lobby_initAudio(void) {
    if (audioInitialized) return;

    sound_jump = LoadSound(SOUNDS_PATH "lobby_jump.wav");
    sound_doubleJump = LoadSound(SOUNDS_PATH "lobby_jump.wav");
    SetSoundPitch(sound_doubleJump, 1.35);
    sound_gameLaunch = LoadSound(SOUNDS_PATH "lobby_gameLaunch.wav");

    sound_doubleJumpMeme = LoadSound(SOUNDS_PATH "meme.wav");

    audioInitialized = true;
}

void lobby_freeAudio(void) {
    if (!audioInitialized) return;
    audioInitialized = false;

    if (IsSoundValid(sound_jump)) UnloadSound(sound_jump);
    if (IsSoundValid(sound_doubleJump)) UnloadSound(sound_doubleJump);
    if (IsSoundValid(sound_gameLaunch)) UnloadSound(sound_gameLaunch);

    if (IsSoundValid(sound_doubleJumpMeme)) UnloadSound(sound_doubleJumpMeme);
}