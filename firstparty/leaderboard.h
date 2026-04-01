/**
 * @file leaderboard.h
 * @brief Gestion des classements locaux (Highscores).
 */

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "baseTypes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEADERBOARD_ENTRIES 100

typedef struct {
    char name[32];
    s32 score;
} LeaderboardEntry_St;

typedef struct {
    LeaderboardEntry_St entries[MAX_LEADERBOARD_ENTRIES];
    s32 count;
} Leaderboard_St;

/**
 * @brief Charge le classement d'un jeu donné.
 */
static inline Leaderboard_St LoadLeaderboard(u8 game_id) {
    Leaderboard_St lb;
    memset(&lb, 0, sizeof(lb));
    
    char path[64];
    sprintf(path, "leaderboard_%d.dat", game_id);
    
    FILE* f = fopen(path, "rb");
    if (f) {
        fread(&lb, sizeof(Leaderboard_St), 1, f);
        fclose(f);
    } else {
        // Data de test si fichier absent
        lb.count = 20;
        for (int i=0; i<20; i++) {
            sprintf(lb.entries[i].name, "Player %d", i+1);
            lb.entries[i].score = (20-i) * 1000;
        }
    }
    return lb;
}

/**
 * @brief Enregistre un nouveau score et retourne le rang final.
 */
static inline s32 SubmitScore(u8 game_id, const char* name, s32 score) {
    Leaderboard_St lb = LoadLeaderboard(game_id);
    s32 rank = lb.count + 1;

    for (int i=0; i<lb.count; i++) {
        if (score > lb.entries[i].score) {
            rank = i + 1;
            for (int j = (lb.count < MAX_LEADERBOARD_ENTRIES-1 ? lb.count : MAX_LEADERBOARD_ENTRIES-1); j > i; j--) {
                lb.entries[j] = lb.entries[j-1];
            }
            strncpy(lb.entries[i].name, name, 31);
            lb.entries[i].name[31] = '\0';
            lb.entries[i].score = score;
            if (lb.count < MAX_LEADERBOARD_ENTRIES) lb.count++;
            break;
        }
    }
    
    if (rank > lb.count && lb.count < MAX_LEADERBOARD_ENTRIES) {
        rank = lb.count + 1;
        strncpy(lb.entries[lb.count].name, name, 31);
        lb.entries[lb.count].name[31] = '\0';
        lb.entries[lb.count].score = score;
        lb.count++;
    }

    char path[64];
    sprintf(path, "leaderboard_%d.dat", game_id);
    FILE* f = fopen(path, "wb");
    if (f) {
        fwrite(&lb, sizeof(Leaderboard_St), 1, f);
        fclose(f);
    }
    
    return rank;
}

#endif
