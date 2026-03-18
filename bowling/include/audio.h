#ifndef BOWLING_AUDIO_H
#define BOWLING_AUDIO_H

#include "utils/configs.h"

extern Sound sound_ballFall;
extern Sound sound_pinFall;
extern Sound sound_strike;

void initAudio(void);
void freeAudio(void);

#endif