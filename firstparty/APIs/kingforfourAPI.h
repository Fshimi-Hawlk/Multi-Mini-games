/**
 * @file kingforfourAPI.h
 * @brief Interface publique pour le mini-jeu King-for-Four.
 * Intégration conforme au standard du Lobby (API Conversion).
 */

#ifndef KINGFORFOUR_API_H
#define KINGFORFOUR_API_H

#include "APIs/generalAPI.h"

// ────────────────────────────────────────────────────────────────────────────
// Types
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Déclaration anticipée opaque. La définition interne est privée.
 */
typedef struct KingForFourGame_St KingForFourGame_St;

/**
 * @brief Paramètres de configuration pour l'initialisation.
 */
typedef struct {
    void *__useless;
    unsigned int fps;           ///< Fréquence cible
} KingForFourConfigs_St;

/**
 * @brief Macro de commodité pour l'initialisation.
 */
#define kingforfour_initGame(game, ...) \
    kingforfour_initGame__full((game), (KingForFourConfigs_St){ .fps = 60, __VA_ARGS__ })

// ────────────────────────────────────────────────────────────────────────────
// API de Cycle de Vie
// ────────────────────────────────────────────────────────────────────────────

/**
 * @brief Alloue et initialise une instance de King-for-Four.
 */
Error_Et kingforfour_initGame__full(KingForFourGame_St** game_ptr, KingForFourConfigs_St configs);

/**
 * @brief Exécute une frame logique complète (Input -> Update -> Render).
 */
Error_Et kingforfour_gameLoop(KingForFourGame_St* const game);

/**
 * @brief Libère toutes les ressources allouées par le jeu.
 */
Error_Et kingforfour_freeGame(KingForFourGame_St** game);

#endif // KINGFORFOUR_API_H