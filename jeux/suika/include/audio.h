#ifndef SUIKA_AUDIO_H
#define SUIKA_AUDIO_H

#include "utils/configs.h"
#include "utils/common.h"

extern Sound sound_drop;
extern Sound sound_merge;

void suika_initAudio(void);
void suika_freeAudio(void);

#endif
