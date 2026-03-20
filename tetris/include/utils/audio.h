#ifndef TETRIS_AUDIO_H
#define TETRIS_AUDIO_H

#include "configs.h"
#include "common.h"

extern Sound sound_lineClear;
extern Sound sound_shapePlaced;

void tetris_initAudio(void);
void tetris_freeAudio(void);

#endif