/**
    @file suikaAPI.h
    @author Multi Mini-Games Team
    @date February 2026
    @brief Public API for the Suika (Watermelon Game) mini-game.

    Defines the opaque game handle and the minimal lifecycle functions
    required to integrate Suika into the lobby.

    @note The internal structure `SuikaGame_St` is **opaque** outside this module.
          Direct field access from the lobby or other modules is forbidden.

    @see generalAPI.h for the required `BaseGame_St` base structure and `Error_Et` codes
*/

#ifndef SUIKA_API_H
#define SUIKA_API_H

#include "APIs/generalAPI.h"

typedef struct SuikaGame_St SuikaGame_St;

typedef struct
{
    unsigned int fps;
} SuikaConfigs_St;

#define suika_initGame(game, ...) \
    suika_initGame__full((game), (SuikaConfigs_St){ .fps = 60, __VA_ARGS__ })

Error_Et suika_initGame__full(SuikaGame_St** game_ptr, SuikaConfigs_St configs);

Error_Et suika_gameLoop(SuikaGame_St* const game);

Error_Et suika_freeGame(SuikaGame_St** game);

#endif
