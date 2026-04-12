#define _USE_DEFAULT_ALLOC
#include "leaderboard.h"
#include "APIs/generalAPI.h"

#include <stdio.h>
#include <string.h>

Leaderboard_St leaderboard_load(MiniGameId_Et gameId) {
    Leaderboard_St lb = {0};
    
    char path[64];
    snprintf(path, sizeof(path), "assets/gameData/leaderboard_%d.dat", gameId);
    
    FILE* f = fopen(path, "rb");
    if (f) {
        fread(&lb, sizeof(Leaderboard_St), 1, f);
        fclose(f);
    } else {
        // Data de test si fichier absent
        u32 testCount = 10;
        for (u32 i = 0; i < 10; i++) {
            snprintf(lb.items[i].name, sizeof(lb.items[i].name), "Player %d", i+1);
            lb.items[i].score = (testCount - i) * 1000;
        }
    }
    return lb;
}

Error_Et leaderboard_save(const Leaderboard_St* const leaderboard, MiniGameId_Et gameId) {
    char path[64];
    snprintf(path, sizeof(path), "leaderboard_%d.dat", gameId);
    FILE* f = fopen(path, "wb");
    if (f == NULL) return ERROR_NULL_POINTER;

    fwrite(leaderboard, sizeof(Leaderboard_St), 1, f);
    fclose(f);

    return OK;
}

u32 leaderboard_submitScore(MiniGameId_Et gameId, const char* name, s32 score) {
    Leaderboard_St lb = leaderboard_load(gameId);
    u32 rank = lb.count + 1;

    for (u32 i = 0; i < lb.count; i++) {
        if (score < lb.items[i].score) continue;
        rank = i + 1;

        strncpy(lb.items[i].name, name, sizeof(lb.items[i].name));
        lb.items[i].name[31] = '\0';

        lb.items[i].score = score;

        for (u32 j = lb.count; j > i; j--) {
            lb.items[j] = lb.items[j-1];
        }

        break;
    }
    
    if (rank > lb.count) {
        rank = lb.count + 1;
        LeaderboardEntry_St entry = {
            .score = score
        };
        strncpy(entry.name, name, sizeof(entry.name));
        entry.name[31] = '\0';

        da_append(&lb, entry);
    }

    leaderboard_save(&lb, gameId);
    
    return rank;
}
