#ifndef UTILS_AUDIO_H
#define UTILS_AUDIO_H

#include "utils/config.h"
#include "utils/common.h"

extern Sound sound_check;
extern Sound sound_checkMate;
extern Sound sound_move;
extern Sound sound_promotion;

void chess_initAudio(void);
void chess_freeAudio(void);

#endif
