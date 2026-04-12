/**
 * @file leaderboard.h
 * @brief Gestion des classements locaux (Highscores).
 */

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"

#include "nob/dynamicArray.h"

typedef struct {
    char name[32];
    s32 score;
} LeaderboardEntry_St;

typeDA(LeaderboardEntry_St, Leaderboard_St);

/**
 * @brief Charge le classement d'un jeu donné.
 */
Leaderboard_St leaderboard_load(MiniGameId_Et gameId);

/**
 * @brief Enregistre un nouveau score et retourne le rang final.
 */
u32 leaderboard_submitScore(MiniGameId_Et gameId, const char* name, s32 score);

Error_Et leaderboard_save(const Leaderboard_St* const leaderboard, MiniGameId_Et gameId);

#endif
