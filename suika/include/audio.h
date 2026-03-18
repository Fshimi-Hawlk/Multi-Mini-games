#ifndef SUIKA_AUDIO_H
#define SUIKA_AUDIO_H

#include "utils/configs.h"
#include "utils/common.h"

extern Sound sound_drop;
extern Sound sound_merge;

void initAudio(void);
void freeAudio(void);

#endif