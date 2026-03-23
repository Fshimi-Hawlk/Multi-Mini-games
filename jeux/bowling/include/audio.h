#ifndef BOWLING_AUDIO_H
#define BOWLING_AUDIO_H

#include "utils/configs.h"

extern Sound sound_ballFall;
extern Sound sound_pinFall;
extern Sound sound_strike;

void bowling_initAudio(void);
void bowling_freeAudio(void);

#endif