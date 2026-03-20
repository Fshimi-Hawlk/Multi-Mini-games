#ifndef LOBBY_AUDIO_H
#define LOBBY_AUDIO_H

#include "config.h"
#include "common.h"

extern Sound sound_jump;
extern Sound sound_doubleJump;
extern Sound sound_gameLaunch;

extern Sound meme;

void lobby_initAudio(void);
void lobby_freeAudio(void);

#endif