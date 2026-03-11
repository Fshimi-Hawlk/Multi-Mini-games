/**
 * @file kingforfourAPI.c
 * @brief Implémentation du sous-jeu King-for-Four respectant BaseGame_St.
 */

#include "core/game.h"
#include "core/card.h"
#include "core/player.h"

// Suppose la présence de raylib ou autre utilitaire visuel du jeu
#include "raylib.h"
#include "logger.h"

#include "APIs/generalAPI.h"
#include "kingforfourAPI.h"

/**
 * @brief État concret du jeu King-for-Four.
 * Le membre `base` DOIT être le premier pour le downcast.
 */
struct KingForFourGame_St {
    BaseGame_St base;       // Obligatoire et en première position
    GameState state;        // Logique métier existante du Uno
    // Vous pouvez ajouter d'autres variables d'interface utilisateur ici
};

Error_Et kingforfour_freeGameWrapper(void* game) {
    return kingforfour_freeGame((KingForFourGame_St**) game);
}

Error_Et kingforfour_initGame__full(KingForFourGame_St** game, KingForFourConfigs_St configs) {
    (void)configs;

    *game = malloc(sizeof(KingForFourGame_St));
    if (*game == NULL) {
        log_error("Échec de l'allocation pour KingForFourGame_St");
        return ERROR_ALLOC;
    }

    KingForFourGame_St* gameRef = *game;
    memset(gameRef, 0, sizeof(*gameRef));

    // Initialisation BaseGame_St
    gameRef->base.freeGame = kingforfour_freeGameWrapper;
    gameRef->base.running  = true;
    gameRef->base.score    = 0;

    // Initialisation Logique Métier King-for-Four
    init_game_logic(&gameRef->state);
    init_uno_deck(&gameRef->state.draw_pile);
    human_shuffle_deck(&gameRef->state.draw_pile);
    
    gameRef->state.num_players = 1; // Joueur Local par défaut
    init_player(&gameRef->state.players[0], 0, "Joueur Local");
    distribute_cards(&gameRef->state);

    log_debug("King-for-Four initialisé avec succès");
    return OK;
}

Error_Et kingforfour_gameLoop(KingForFourGame_St* const game) {
    if (game == NULL) {
        log_error("Pointeur de jeu NULL dans gameLoop");
        return ERROR_NULL_POINTER;
    }
    
    if (!game->base.running) return OK;

    // --- GESTION INPUT (Exemple adapté de votre ancien main.c) ---
    if (IsKeyPressed(KEY_ESCAPE)) {
        game->base.running = false; // Permet au lobby de reprendre la main
        return OK;
    }

    // (La logique de clic sur la main ou la pioche devrait être insérée ici)

    // --- RENDU RAYLIB ---
    // Ne pas appeler InitWindow ni CloseWindow, uniquement Begin/EndDrawing
    BeginDrawing();
        ClearBackground((Color){0, 100, 0, 255}); // Fond vert

        // Appels à vos fonctions de rendu existantes
        // RenderTable(&game->state, assets);
        // RenderHand(&game->state.players[0], assets);
        
        DrawText("ECHAP pour quitter vers le Lobby", 10, 30, 20, WHITE);
    EndDrawing();

    return OK;
}

Error_Et kingforfour_freeGame(KingForFourGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    KingForFourGame_St* gameRef = *game;

    // Nettoyage de la logique métier
    free_deck(&gameRef->state.draw_pile);
    free_deck(&gameRef->state.discard_pile);
    for(int i = 0; i < gameRef->state.num_players; i++) {
        free_deck(&gameRef->state.players[i].hand);
    }

    free(gameRef);
    *game = NULL;

    log_debug("King-for-Four détruit avec succès");
    return OK;
}