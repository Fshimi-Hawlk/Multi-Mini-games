/**
    @file globals.h
    @author Léandre BAUDET
    @date 2026-04-14
    @date 2026-04-14
    @brief External global variables for the Tetromino Fall game.
*/
#ifndef UTILS_GLOBALS_H
#define UTILS_GLOBALS_H

#include "utils/types.h"

extern inputRepeat_st inputRepeat;      ///< Global state for input key repetition timing.

extern Color tetraminosColors[];        ///< Predefined colors for each tetromino shape.
extern tetramino tetraminosShapes[];    ///< Predefined shapes for each tetromino type.

#endif
