/**
    @file audio.c
    @author Léandre BAUDET
    @date 2026-04-02
    @date 2026-04-14
    @brief Audio management for Chess.
*/
#include "audio.h"

/**
    @brief Flag indicating if the audio system has been initialized.
*/
static bool audioInitialized = false;

Sound sound_check;       ///< Sound played when a king is in check
Sound sound_checkMate;   ///< Sound played when checkmate occurs
Sound sound_move;        ///< Sound played when a piece is moved
Sound sound_promotion;   ///< Sound played when a pawn is promoted

/**
    @brief Initialize the chess audio system and load sound files.
*/
void chess_initAudio(void) {
    if (audioInitialized) return;

    sound_check = LoadSound(SOUNDS_PATH "chess_check.wav");
    sound_checkMate = LoadSound(SOUNDS_PATH "chess_checkMate.wav");
    sound_move = LoadSound(SOUNDS_PATH "chess_move.wav");
    sound_promotion = LoadSound(SOUNDS_PATH "chess_promotion.wav");

    audioInitialized = true;
}

/**
    @brief Free all chess sound resources and shut down the audio system.
*/
void chess_freeAudio(void) {
    if (!audioInitialized) return;

    if (IsSoundValid(sound_check)) UnloadSound(sound_check);
    if (IsSoundValid(sound_checkMate)) UnloadSound(sound_checkMate);
    if (IsSoundValid(sound_move)) UnloadSound(sound_move);
    if (IsSoundValid(sound_promotion)) UnloadSound(sound_promotion);

    audioInitialized = false;
}
