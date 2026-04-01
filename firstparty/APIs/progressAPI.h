/**
 * @file progressAPI.h
 * @author i-Charlys
 * @date 2026-03-22
 * @brief API for handling player progression, skins, AP, and rewards.
 */

#ifndef PROGRESS_API_H
#define PROGRESS_API_H

#include "baseTypes.h"
#include <stdbool.h>

/** @brief Ability Potential tiers. */
typedef enum {
    AP_COMMON,     /**< Baseline */
    AP_UNCOMMON,   /**< Noticeable improvement */
    AP_RARE,       /**< Strong */
    AP_LEGENDARY,  /**< Very powerful */
    AP_MYSTICAL,   /**< Max power for a single game */
    AP_PLUS_ULTRA  /**< Lobby-only maximum power */
} AP_Tier_Et;

/** @brief Max number of games supported in progression. */
#define MAX_GAMES_PROGRESS 16
/** @brief Max variants per game. */
#define MAX_VARIANTS 8
/** @brief Max difficulties per game. */
#define MAX_DIFFICULTIES 4

/** @brief Structure representing a skin. */
typedef struct {
    u8 game_id;
    u8 variant_id;
    u8 difficulty_id;
    char name[32];
    char ability_name[32];
} Skin_St;

/** @brief Global player progress structure. */
typedef struct {
    bool skin_unlocked[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    s32 high_scores[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    s32 leaderboard_rank[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES]; // Current rank
    s32 total_players[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];   // Total players in leaderboard
    
    bool has_crown;
    bool jewel_unlocked[MAX_GAMES_PROGRESS][MAX_VARIANTS][MAX_DIFFICULTIES];
    
    // Derived state
    AP_Tier_Et current_ap[MAX_GAMES_PROGRESS];
} PlayerProgress_St;

/**
 * @brief Calculates the AP tier based on leaderboard placement.
 */
static inline AP_Tier_Et CalculateAPTier(s32 rank, s32 total) {
    if (total <= 0) return AP_COMMON;
    if (rank == 1) return AP_LEGENDARY; // Simplified, #1 logic elsewhere
    
    f32 percentile = (f32)rank / (f32)total;
    if (percentile <= 0.25f) return AP_LEGENDARY;
    if (percentile <= 0.50f) return AP_RARE;
    if (percentile <= 0.75f) return AP_UNCOMMON;
    return AP_COMMON;
}

/**
 * @brief Saves the progress to a local file.
 */
void SaveProgress(const PlayerProgress_St* progress);

/**
 * @brief Loads the progress from a local file.
 */
PlayerProgress_St LoadProgress(void);

#endif // PROGRESS_API_H
