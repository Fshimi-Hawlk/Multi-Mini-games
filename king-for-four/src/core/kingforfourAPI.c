/**
 * @file kingforfourAPI.c
 * @brief Implémentation complète du jeu King-for-Four via API.
 * Encapsule la machine d'état, Raylib, et les règles du Uno.
 */

#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

#include "core/game.h"
#include "core/card.h"
#include "core/player.h"
#include "ui/renderer.h"

#include "logger.h"
#include "APIs/generalAPI.h"
#include "kingforfourAPI.h"

// --- DÉFINITION DES ÉTATS ---
typedef enum {
    STATE_MENU,
    STATE_GAME
} AppState;

/**
 * @brief État concret complet du jeu King-for-Four.
 */
struct KingForFourGame_St {
    BaseGame_St base;       // Obligatoire, permet le cast par le Lobby
    
    GameState state;        // Données métier du Uno (pioche, mains, etc.)
    GameAssets assets;      // Textures Raylib
    AppState currentState;  // Gestion Menu / Jeu
};

// Wrapper pour correspondre à la signature demandée par generalAPI.h
Error_Et kingforfour_freeGameWrapper(void* game) {
    return kingforfour_freeGame((KingForFourGame_St**) game);
}

Error_Et kingforfour_initGame__full(KingForFourGame_St** game, KingForFourConfigs_St configs) {
    (void)configs;

    *game = calloc(1, sizeof(KingForFourGame_St));
    if (*game == NULL) {
        log_error("Erreur d'allocation pour KingForFourGame_St");
        return ERROR_ALLOC;
    }

    KingForFourGame_St* g = *game;

    // 1. Initialisation Base
    g->base.freeGame = kingforfour_freeGameWrapper;
    g->base.running  = true;
    g->base.score    = 0;

    // 2. Initialisation Environnement
    srand(time(NULL));
    
    // Le Lobby a déjà appelé InitWindow(), on peut donc charger les assets en sécurité
    g->assets = LoadAssets();
    g->currentState = STATE_MENU;

    log_debug("King-for-Four : Instance allouée et assets chargés.");
    return OK;
}

Error_Et kingforfour_gameLoop(KingForFourGame_St* const game) {
    if (game == NULL) return ERROR_NULL_POINTER;
    if (!game->base.running) return OK;

    // ============================
    //     LOGIQUE (UPDATE)
    // ============================
    switch (game->currentState) {
        case STATE_MENU:
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)) {
                // --- NOUVELLE PARTIE ---
                init_game_logic(&game->state);
                init_uno_deck(&game->state.draw_pile);
                
                human_shuffle_deck(&game->state.draw_pile); 
                
                game->state.num_players = 1;
                init_player(&game->state.players[0], 0, "Joueur Local");
                distribute_cards(&game->state);
                
                game->currentState = STATE_GAME;
            }
            
            // Échap dans le menu = On demande au Lobby de reprendre la main
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->base.running = false; 
                return OK;
            }
            break;

        case STATE_GAME:
            // --- GESTION DU JEU ---
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                int clickedHandIndex = GetHoveredCardIndex(&game->state.players[0], game->assets);
                if (clickedHandIndex != -1) {
                    try_play_card(&game->state, 0, clickedHandIndex);
                }
                else {
                    Rectangle deckRect = GetDeckRect(game->assets);
                    if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                        player_draw_card(&game->state, 0);
                    }
                }
            }
            
            // Retour au menu du mini-jeu avec Echap
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->currentState = STATE_MENU;
            }
            break;
    }

    // Gestion Plein écran
    if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
         if (IsWindowState(FLAG_FULLSCREEN_MODE)) ToggleFullscreen();
         ToggleBorderlessWindowed();
    }

    // ============================
    //      DESSIN (DRAW)
    // ============================
    BeginDrawing();
        ClearBackground((Color){0, 100, 0, 255}); // Fond vert tapis

        switch (game->currentState) {
            case STATE_MENU:
                RenderMenu(); // Affiche le titre
                DrawText("Appuyez sur ECHAP pour retourner au Lobby central", 10, 10, 20, LIGHTGRAY);
                break;

            case STATE_GAME:
                RenderTable(&game->state, game->assets);
                RenderHand(&game->state.players[0], game->assets);
                
                // HUD
                DrawFPS(10, 10);
                DrawText("ECHAP pour Menu Principal", 10, 30, 20, WHITE);
                
                const char* cColors[] = {"ROUGE", "JAUNE", "VERT", "BLEU", "NOIR"};
                const char* activeColor = (game->state.active_color >= 0 && game->state.active_color <= 4) ? cColors[game->state.active_color] : "---";
                DrawText(TextFormat("Couleur Active: %s", activeColor), 10, 60, 20, WHITE);
                break;
        }
    EndDrawing();

    return OK;
}

Error_Et kingforfour_freeGame(KingForFourGame_St** game) {
    if (game == NULL || *game == NULL) return ERROR_NULL_POINTER;
    KingForFourGame_St* g = *game;

    // 1. Nettoyage de la mémoire métier
    free_deck(&g->state.draw_pile);
    free_deck(&g->state.discard_pile);
    for(int i = 0; i < g->state.num_players; i++) {
        free_deck(&g->state.players[i].hand);
    }

    // 2. Déchargement de la mémoire graphique (VRAM)
    UnloadAssets(g->assets);

    // 3. Libération de l'instance
    free(g);
    *game = NULL;

    log_debug("King-for-Four : Ressources libérées avec succès.");
    return OK;
}