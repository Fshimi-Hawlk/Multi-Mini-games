/**
    @file progress.c
    @author Multi Mini-Games Team
    @date 2026-03-22
    @date 2026-04-14
    @brief Implementation of player progress saving/loading.
*/
#include "progress.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

// Helper to write a boolean as a byte
static void write_bool(FILE* f, bool b) {
    u8 val = b ? 1 : 0;
    fwrite(&val, 1, 1, f);
}

// Helper to read a byte as a boolean
static bool read_bool(FILE* f) {
    u8 val = 0;
    if (fread(&val, 1, 1, f) != 1) return false;
    return val != 0;
}

// Helper to write s32 in network byte order
static void write_s32(FILE* f, s32 v) {
    u32 net_v = htonl((u32)v);
    fwrite(&net_v, 4, 1, f);
}

// Helper to read s32 from network byte order
static s32 read_s32(FILE* f) {
    u32 net_v = 0;
    if (fread(&net_v, 4, 1, f) != 1) return 0;
    return (s32)ntohl(net_v);
}

void SaveProgress(const PlayerProgress_St* progress) {
    FILE* f = fopen(PROGRESS_FILE, "wb");
    if (!f) return;

    for (int g = 0; g < MAX_GAMES_PROGRESS; g++) {
        for (int v = 0; v < MAX_VARIANTS; v++) {
            for (int d = 0; d < MAX_DIFFICULTIES; d++) {
                write_bool(f, progress->skin_unlocked[g][v][d]);
                write_s32(f, progress->high_scores[g][v][d]);
                write_s32(f, progress->leaderboard_rank[g][v][d]);
                write_s32(f, progress->total_players[g][v][d]);
                write_bool(f, progress->jewel_unlocked[g][v][d]);
            }
        }
    }

    write_bool(f, progress->has_crown);

    for (int g = 0; g < MAX_GAMES_PROGRESS; g++) {
        write_s32(f, (s32)progress->current_ap[g]);
    }

    fclose(f);
}

PlayerProgress_St LoadProgress(void) {
    PlayerProgress_St progress;
    memset(&progress, 0, sizeof(PlayerProgress_St));
    
    FILE* f = fopen(PROGRESS_FILE, "rb");
    if (!f) {
        progress.has_crown = false;
        for (s32 g = 0; g < MAX_GAMES_PROGRESS; g++) {
            progress.current_ap[g] = AP_COMMON;
        }
        return progress;
    }

    for (int g = 0; g < MAX_GAMES_PROGRESS; g++) {
        for (int v = 0; v < MAX_VARIANTS; v++) {
            for (int d = 0; d < MAX_DIFFICULTIES; d++) {
                progress.skin_unlocked[g][v][d] = read_bool(f);
                progress.high_scores[g][v][d] = read_s32(f);
                progress.leaderboard_rank[g][v][d] = read_s32(f);
                progress.total_players[g][v][d] = read_s32(f);
                progress.jewel_unlocked[g][v][d] = read_bool(f);
            }
        }
    }

    progress.has_crown = read_bool(f);

    for (int g = 0; g < MAX_GAMES_PROGRESS; g++) {
        progress.current_ap[g] = (AP_Tier_Et)read_s32(f);
    }

    fclose(f);
    return progress;
}
