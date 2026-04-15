/**
    @file globals.h
    @author Fshimi-Hawlk
    @date 2026-02-06
    @date 2026-04-14
    @brief External global variables for the Tetromino Fall game.
*/
#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "utils/types.h"

extern InputRepeat_St inputRepeat;      ///< Global state for input key repetition timing.

extern Color tetraminosColors[];        ///< Predefined colors for each tetromino shape.
extern Tetromino_t tetraminosShapes[];    ///< Predefined shapes for each tetromino type.

#endif
