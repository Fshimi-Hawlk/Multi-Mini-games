/**
    @file leaderboard.h
    @author I-Charlys
    @author Fshimi-Hawlk
    @date 2026-04-13
    @brief Local highscore / leaderboard management using a dynamic array.

    Supports loading, saving, submitting scores, and creating filtered views
    for UI display (e.g. "All", "Top 10", "My scores", etc.).
*/

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "baseTypes.h"
#include "APIs/generalAPI.h"
#include "nob/dynamicArray.h"

typedef struct {
    char name[32];
    s32  score;
} LeaderboardEntry_St;

typeDA(LeaderboardEntry_St, Leaderboard_St);

// ────────────────────────────────────────────────
// Sorting
// ────────────────────────────────────────────────

/**
    @brief Sort order for leaderboard entries.
*/
typedef enum {
    LEADERBOARD_SORT_DESCENDING,   ///< Highest score first (default)
    LEADERBOARD_SORT_ASCENDING,    ///< Lowest score first
    __leaderboardSortCount
} LeaderboardSort_Et;

/**
    @brief Sort the leaderboard in-place using the specified order.
*/
void leaderboard_sort(Leaderboard_St* lb, LeaderboardSort_Et order);

// ────────────────────────────────────────────────
// Core API
// ────────────────────────────────────────────────

/**
    @brief Load leaderboard for a given mini-game from disk.
    @return Dynamic array containing all entries (sorted descending by score).
            Returns empty array if file doesn't exist or on error.
*/
Leaderboard_St leaderboard_load(MiniGameId_Et gameId);

/**
    @brief Submit a new score, insert it in the correct position, save to disk,
           and return the final 1-based rank of the player.
*/
u32 leaderboard_submitScore(MiniGameId_Et gameId, const char* name, s32 score);

/**
    @brief Save the entire leaderboard to disk.
    @return OK on success, ERROR_... otherwise.
*/
Error_Et leaderboard_save(const Leaderboard_St* const leaderboard, MiniGameId_Et gameId);

// ────────────────────────────────────────────────
// Filtered list view support (for UI)
// ────────────────────────────────────────────────

/**
    @brief Create a filtered view of the leaderboard.
           The returned array contains pointers to entries from the original
           leaderboard (no copying of data, very cheap).
           Caller is responsible for calling da_free() on the result when done.
*/
typeDA(LeaderboardEntry_St*, LeaderboardView_St);

/**
    @brief Create a view containing all entries (full leaderboard).
*/
LeaderboardView_St leaderboardView_all(const Leaderboard_St* lb);

/**
    @brief Create a view with only the top N entries.
*/
LeaderboardView_St leaderboardView_topN(const Leaderboard_St* lb, u32 n);

/**
    @brief Create a view filtered by a custom predicate.
           Example: show only entries where name starts with a certain letter.
*/
typedef bool (*LeaderboardFilter_Ft)(const LeaderboardEntry_St* entry, void* userData);

LeaderboardView_St leaderboardView_filter(
    const Leaderboard_St* lb,
    LeaderboardFilter_Ft  filter,
    void*                 userData
);

#endif // LEADERBOARD_H