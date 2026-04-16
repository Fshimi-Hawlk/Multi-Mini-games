/**
    @file audio.h
    @author Fshimi-Hawlk
    @date 2026-03-20
    @date 2026-04-14
    @brief Audio initialization and cleanup function declarations for Poly Blast.
*/

#ifndef POLY_BLAST_SETUPS_AUDIO_H
#define POLY_BLAST_SETUPS_AUDIO_H

/**
    @brief Initializes all audio resources and sound effects for the game.
*/
void polyBlast_initAudio(void);

/**
    @brief Unloads all audio resources and shuts down the audio system.
*/
void polyBlast_freeAudio(void);

#endif // POLY_BLAST_SETUPS_AUDIO_H
