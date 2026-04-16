/**
    @file audio.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Audio management for the Suika game.
*/
#ifndef SUIKA_AUDIO_H
#define SUIKA_AUDIO_H

#include "utils/configs.h"
#include "utils/common.h"

extern Sound sound_drop;  ///< Sound played when a fruit is dropped
extern Sound sound_merge; ///< Sound played when two fruits merge

/**
    @brief Initialize audio resources for the Suika game.

    Loads all required sound files from disk.
    @return void
*/
void suika_initAudio(void);

/**
    @brief Free all audio resources used by the Suika game.

    Unloads sounds and resets audio state.
    @return void
*/
void suika_freeAudio(void);

#endif
