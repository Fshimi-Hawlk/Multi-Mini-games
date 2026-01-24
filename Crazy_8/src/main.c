#include "raylib.h"
#include "../include/game.h"
#include "../include/UI.h"
#include <stdio.h>
#include <time.h> // Pour le srand

int main(void)
{
    // 1. Configuration de la fenêtre
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Crazy 8 - Raylib Implementation");
    SetTargetFPS(60);

    // 2. Initialisation de l'aléatoire
    srand(time(NULL));

    // 3. Chargement des Assets (Textures)
    GameAssets assets = LoadAssets();

    // 4. Initialisation du Jeu (Logique)
    GameState game = {0}; 
    init_game_logic(&game);

    // Création du deck et mélange
    init_uno_deck(&game.draw_pile);
    shuffle_deck(&game.draw_pile);

    // Création du joueur local (Toi)
    game.num_players = 1; // Pour l'instant juste toi
    init_player(&game.players[0], 0, "Joueur Local");

    // Distribution des cartes (7 par défaut)
    distribute_cards(&game);

    // --- BOUCLE PRINCIPALE ---
    while (!WindowShouldClose())
    {
        // --- MISE À JOUR (Update) ---
        // Ici on mettra plus tard la détection des clics souris
        
        // --- DESSIN (Draw) ---
        BeginDrawing();
            
            // Fond d'écran vert tapis de jeu
            ClearBackground((Color){0, 100, 0, 255}); 

            // Affiche la table (Pioche + Carte posée)
            RenderTable(&game, assets);

            // Affiche la main du joueur 0 (Toi)
            RenderHand(&game.players[0], assets);

            // Petit texte d'info
            DrawText("Crazy 8 / Uno - Demo", 10, 10, 20, WHITE);
            DrawFPS(1200, 10);

        EndDrawing();
    }

    // --- NETTOYAGE ---
    // Libération de la mémoire des listes chaînées
    free_deck(&game.draw_pile);
    free_deck(&game.discard_pile);
    for(int i=0; i<game.num_players; i++) {
        free_deck(&game.players[i].hand);
    }

    // Libération des textures
    UnloadAssets(assets);
    
    CloseWindow();

    return 0;
}