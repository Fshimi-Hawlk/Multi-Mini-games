/**
    @file blockBlastAPI.h
    @author Multi Mini-Games Team
    @date 2026-04-05
    @brief Public API for the Block-Blast mini-game.
*/

#ifndef BLOCK_BLAST_API_H
#define BLOCK_BLAST_API_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

/* ── Opaque handle ────────────────────────────────────────── */
typedef struct BlockBlastAPI_St BlockBlastAPI_St;

/* ── Config placeholder ───────────────────────────────────── */
typedef struct { char _; } BlockBlastConfigs_St;

#define blockBlast_initGame(game, ...) \
    blockBlast_initGame__full((game), (BlockBlastConfigs_St){ ._ = 0, ##__VA_ARGS__ })

/* ── Lifecycle ────────────────────────────────────────────── */
Error_Et blockBlast_initGame__full(BlockBlastAPI_St** game, BlockBlastConfigs_St configs);
Error_Et blockBlast_gameLoop(BlockBlastAPI_St* const game);
Error_Et blockBlast_freeGame(BlockBlastAPI_St** game);
bool     blockBlast_isRunning(const BlockBlastAPI_St* game);

#endif // BLOCK_BLAST_API_H
