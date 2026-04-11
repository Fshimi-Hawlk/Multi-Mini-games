#ifndef CHESS_AUDIO_H
#define CHESS_AUDIO_H

#include "config.h"
#include "common.h"

extern Sound sound_check;
extern Sound sound_checkMate;
extern Sound sound_move;
extern Sound sound_promotion;

void chess_initAudio(void);
void chess_freeAudio(void);

#endif