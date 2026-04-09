/**
 * @file progress.h
 * @brief Gestion de la progression, des skins et de l'Ability Potential (AP).
 */

#ifndef PROGRESS_H
#define PROGRESS_H

#include "baseTypes.h"
#include <stdbool.h>

/** @brief Paliers d'Ability Potential. */
typedef enum {
    AP_COMMON,
    AP_UNCOMMON,
    AP_RARE,
    AP_LEGENDARY,
    AP_MYSTICAL,
    AP_PLUS_ULTRA  /**< Uniquement pour le lobby */
} AP_Tier_Et;

#define MAX_GAMES_PROGRESS 16
#define MAX_VARIANTS 8
#define MAX_DIFFICULTIES 4
#define PROGRESS_FILE "progress.dat"

typedef struct {
    u8 game_id;
    u8 variant_id;
    u8 difficulty_id;
    char name[32];
    char ability_name[32];
} Skin_St;

typedef struct {
    bool skin_unlocked[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    s32 high_scores[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    s32 leaderboard_rank[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES]; 
    s32 total_players[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];   
    
    bool has_crown;
    bool jewel_unlocked[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    
    AP_Tier_Et current_ap[MAX_GAMES_PROGRESS];
} PlayerProgress_St;

/**
 * @brief Calcule le palier d'AP selon le quartile du classement.
 */
static inline AP_Tier_Et CalculateAPTier(s32 rank, s32 total) {
    if (total <= 0) return AP_COMMON;
    if (rank == 1) return AP_LEGENDARY;
    
    float percentile = (float)rank / (float)total;
    if (percentile <= 0.25f) return AP_LEGENDARY;
    if (percentile <= 0.50f) return AP_RARE;
    if (percentile <= 0.75f) return AP_UNCOMMON;
    return AP_COMMON;
}

/**
 * @brief Saves the player progress to disk in a portable, endianness-safe format.
 * @note  Implemented in progress.c (part of libfirstparty).
 */
void SaveProgress(const PlayerProgress_St* progress);

/**
 * @brief Loads the player progress from disk.
 * @note  Implemented in progress.c (part of libfirstparty).
 */
PlayerProgress_St LoadProgress(void);

#endif
