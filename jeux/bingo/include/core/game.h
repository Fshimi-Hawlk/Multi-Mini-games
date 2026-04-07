/**
    @file core/game.h
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-05
    @brief One clear sentence that tells what this file is actually for.

    If the file needs more context than fits in @brief, write 2-5 lines here.
    @note Put warnings, important limitations, "we know it's ugly but...", or future plans here

    // Try to align the `for` for better readability
    // Try to keep the same order of the includes
    Use @see `path/to/related/file.h` when this file depends heavily on another one.
*/

#ifndef CORE_GAME_GAME_H
#define CORE_GAME_GAME_H

#include "utils/userTypes.h"

void bingo_updateGame(BingoGame_St* const game, f32 dt, f32Vector2 mousePos);

#endif // CORE_GAME_GAME_H