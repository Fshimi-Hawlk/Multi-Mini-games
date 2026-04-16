/**
    @file audio.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Audio management for Solitaire
*/
#ifndef SOLITAIRE_AUDIO_H
#define SOLITAIRE_AUDIO_H

#include "configs.h"
#include "common.h"

extern Sound sound_cardTurn;    ///< Sound for turning a card

/**
    @brief Initialize audio
*/
void solitaire_initAudio(void);

/**
    @brief Free audio resources
*/
void solitaire_freeAudio(void);

#endif // SOLITAIRE_AUDIO_H

