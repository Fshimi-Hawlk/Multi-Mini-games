/**
    @file audio.h
    @author Maxime CHAUVEAU
    @date 2026-04-07
    @date 2026-04-14
    @brief Audio management for the Bowling mini-game.
*/
#ifndef BOWLING_AUDIO_H
#define BOWLING_AUDIO_H

#include <raylib.h>
#include "utils/configs.h"

/**
    @brief Sound played when the ball falls onto the lane.
*/
extern Sound sound_ballFall;

/**
    @brief Sound played when a pin is hit and falls.
*/
extern Sound sound_pinFall;

/**
    @brief Sound played when a strike is achieved.
*/
extern Sound sound_strike;

/**
    @brief Initializes the audio system and loads sounds.
*/
void bowling_initAudio(void);

/**
    @brief Frees all loaded audio resources.
*/
void bowling_freeAudio(void);

#endif // BOWLING_AUDIO_H
