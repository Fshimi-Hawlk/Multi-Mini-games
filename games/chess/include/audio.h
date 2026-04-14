/**
    @file audio.h
    @author Léandre BAUDET
    @date 2024-01-01
    @date 2026-04-14
    @brief Audio management for the chess game.
*/
#ifndef CHESS_AUDIO_H
#define CHESS_AUDIO_H

#include "config.h"
#include "common.h"

extern Sound sound_check;       ///< Sound played during check
extern Sound sound_checkMate;   ///< Sound played during checkmate
extern Sound sound_move;        ///< Sound played during a piece move
extern Sound sound_promotion;   ///< Sound played during pawn promotion

/**
    @brief Initializes the audio resources for the chess game.
*/
void chess_initAudio(void);

/**
    @brief Frees the audio resources for the chess game.
*/
void chess_freeAudio(void);

#endif
