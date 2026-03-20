/**
    @file core/game.h
    @author Fshimi-Hawlk
    @date 2026-03-02
    @date 2026-03-19
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

bool bingo_isValidDaub(const CallState_St* const state, const PlayerCard_St* playerCard, uint row, uint col);
bool bingo_hasBingo(const PlayerCard_St* const playerCard);

#endif // CORE_GAME_GAME_H