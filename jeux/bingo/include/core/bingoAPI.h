/**
    @file bingoAPI.h
    @author Multi Mini-Games Team
    @date 2026-04-05
    @brief Public API for the Bingo mini-game.

    Opaque handle + minimal lifecycle interface for lobby integration.
    The handle type is BingoAPI_St (distinct from the internal BingoGame_St).
*/

#ifndef BINGO_API_H
#define BINGO_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

/* ── Opaque handle (distinct from internal BingoGame_St) ─── */
typedef struct BingoAPI_St BingoAPI_St;

/* ── Config placeholder ───────────────────────────────────── */
typedef struct { char _; } BingoConfigs_St;

#define bingo_initGame(game, ...) \
    bingo_initGame__full((game), (BingoConfigs_St){ ._ = 0, ##__VA_ARGS__ })

/* ── Lifecycle ────────────────────────────────────────────── */
Error_Et bingo_initGame__full(BingoAPI_St** game, BingoConfigs_St configs);
Error_Et bingo_gameLoop(BingoAPI_St* const game);
Error_Et bingo_freeGame(BingoAPI_St** game);
bool     bingo_isRunning(const BingoAPI_St* game);

#endif // BINGO_API_H
