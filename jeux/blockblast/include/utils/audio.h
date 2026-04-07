#ifndef BLOCKBLAST_AUDIO_H
#define BLOCKBLAST_AUDIO_H

#include "configs.h"
#include "common.h"

extern Sound sound_brickPlaced;
extern Sound sound_combo;
extern Sound sound_gameOver;
extern Sound sound_lineBreak;

void blockBlast_initAudio(void);
void blockBlast_freeAudio(void);

#endif