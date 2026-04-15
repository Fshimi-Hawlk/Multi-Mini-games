/**
    @file leaderboard.c
    @author Multi Mini-Games Team
    @date 2026-04-14
    @date 2026-04-14
    @brief leaderboard.c implementation/header file
*/
#include "contextArena.h"

#define REALLOC context_realloc     // arena-aware realloc (3-arg)
#define FREE                        // future arena-aware free (disabled for now)
#include "leaderboard.h"
#include "APIs/generalAPI.h"
#include "logger.h"
#include "sharedUtils/mathUtils.h"

#include <stdio.h>
#include <string.h>

// ────────────────────────────────────────────────
// Internal helpers
// ────────────────────────────────────────────────

static const char* getLeaderboardPath(MiniGameId_Et gameId) {
    static char path[64];
    snprintf(path, sizeof(path), "assets/gameData/leaderboard_%d.dat", (int)gameId);
    return path;
}

static void leaderboard_sortInternal(Leaderboard_St* lb, bool descending) {
    if (lb == NULL || lb->count <= 1) return;

    for (u32 i = 1; i < lb->count; ++i) {
        LeaderboardEntry_St key = lb->items[i];
        s32 j = (s32)i - 1;

        while (j >= 0) {
            bool shouldSwap = descending 
                            ? (lb->items[j].score < key.score)
                            : (lb->items[j].score > key.score);

            if (!shouldSwap) break;

            lb->items[j + 1] = lb->items[j];
            --j;
        }
        lb->items[j + 1] = key;
    }
}

// ────────────────────────────────────────────────
// Public API
// ────────────────────────────────────────────────

void leaderboard_sort(Leaderboard_St* lb, LeaderboardSort_Et order) {
    if (lb == NULL) return;

    bool descending = (order == LEADERBOARD_SORT_DESCENDING);
    leaderboard_sortInternal(lb, descending);
}

Leaderboard_St leaderboard_load(MiniGameId_Et gameId) {
    Leaderboard_St lb = {0};

    FILE* f = fopen(getLeaderboardPath(gameId), "rb");
    if (f) {
        u32 count = 0;
        if (fread(&count, sizeof(u32), 1, f) == 1 && count > 0) {
            da_reserve(&lb, count);
            if (fread(lb.items, sizeof(LeaderboardEntry_St), count, f) == count) {
                lb.count = count;
            } else {
                lb.count = 0;
            }
        }
        fclose(f);
    } else {
        // Generate test data when file doesn't exist yet
        u32 testCount = 10;
        da_reserve(&lb, testCount);

        for (u32 i = 0; i < testCount; ++i) {
            LeaderboardEntry_St entry = {0};
            snprintf(entry.name, sizeof(entry.name), "Player %u", i + 1);
            entry.score = (testCount - i) * 1000;
            da_append(&lb, entry);
        }
    }

    leaderboard_sort(&lb, LEADERBOARD_SORT_DESCENDING);
    return lb;
}

Error_Et leaderboard_save(const Leaderboard_St* const leaderboard, MiniGameId_Et gameId) {
    FILE* f = fopen(getLeaderboardPath(gameId), "wb");
    if (f == NULL) {
        log_warn("Failed to open leaderboard file for writing: %s", getLeaderboardPath(gameId));
        return ERROR_NULL_POINTER;
    }

    fwrite(&leaderboard->count, sizeof(u32), 1, f);
    if (leaderboard->count > 0) {
        fwrite(leaderboard->items, sizeof(LeaderboardEntry_St), leaderboard->count, f);
    }

    fclose(f);
    return OK;
}

u32 leaderboard_submitScore(MiniGameId_Et gameId, const char* name, s32 score) {
    if (name == NULL) return 0;

    Leaderboard_St lb = leaderboard_load(gameId);

    // Find insertion point (descending order by score)
    u32 insertIndex = lb.count;
    for (u32 i = 0; i < lb.count; ++i) {
        if (score > lb.items[i].score) {
            insertIndex = i;
            break;
        }
    }

    LeaderboardEntry_St newEntry = {0};
    strncpy(newEntry.name, name, sizeof(newEntry.name) - 1);
    newEntry.name[sizeof(newEntry.name) - 1] = '\0';
    newEntry.score = score;

    if (insertIndex < lb.count) {
        da_resize(&lb, lb.count + 1);
        for (u32 i = lb.count - 1; i > insertIndex; --i) {
            lb.items[i] = lb.items[i - 1];
        }
        lb.items[insertIndex] = newEntry;
    } else {
        da_append(&lb, newEntry);
    }

    leaderboard_sort(&lb, LEADERBOARD_SORT_DESCENDING);
    leaderboard_save(&lb, gameId);

    u32 rank = insertIndex + 1;
    da_free(lb);

    return rank;
}

// ────────────────────────────────────────────────
// Filtered List Views
// ────────────────────────────────────────────────

LeaderboardView_St leaderboardView_all(const Leaderboard_St* lb) {
    LeaderboardView_St view = {0};
    if (lb == NULL || lb->count == 0) return view;

    da_reserve(&view, lb->count);
    for (u32 i = 0; i < lb->count; ++i) {
        da_append(&view, &lb->items[i]);
    }
    return view;
}

LeaderboardView_St leaderboardView_topN(const Leaderboard_St* lb, u32 n) {
    LeaderboardView_St view = {0};
    if (lb == NULL || lb->count == 0) return view;

    u32 limit = min(n, lb->count);
    da_reserve(&view, limit);

    for (u32 i = 0; i < limit; ++i) {
        da_append(&view, &lb->items[i]);
    }
    return view;
}

LeaderboardView_St leaderboardView_filter(
    const Leaderboard_St* lb,
    LeaderboardFilter_Ft  filter,
    void*                 userData
) {
    LeaderboardView_St view = {0};
    if (lb == NULL || filter == NULL) return view;

    da_reserve(&view, lb->count);   // worst-case size

    for (u32 i = 0; i < lb->count; ++i) {
        if (filter(&lb->items[i], userData)) {
            da_append(&view, &lb->items[i]);
        }
    }
    return view;
}