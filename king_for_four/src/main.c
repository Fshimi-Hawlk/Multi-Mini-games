#include "raylib.h"
#include <stdio.h>
#include <time.h>

#include "core/game.h"   
#include "ui/renderer.h"

// --- DÉFINITION DES ÉTATS (Menu vs Jeu) ---
typedef enum {
    STATE_MENU,
    STATE_GAME
} AppState;

int main(void)
{
    // --- 1. CONFIGURATION ---
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "King For Four - Raylib");
    SetTargetFPS(144);
    srand(time(NULL)); // Important pour le mélange aléatoire

    // --- 2. INITIALISATION ---
    GameAssets assets = LoadAssets();
    AppState currentState = STATE_MENU; // On commence sur le menu
    
    GameState game = {0}; 
    // On n'initialise pas le jeu tout de suite, on attend le clic "JOUER"

    // --- 3. BOUCLE PRINCIPALE ---
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        
        // ============================
        //     LOGIQUE (UPDATE)
        // ============================
        
        switch (currentState) {
            case STATE_MENU:
                // Si on clique n'importe où (ou sur Entrée), on lance le jeu
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)) {
                    
                    // --- NOUVELLE PARTIE ---
                    init_game_logic(&game);
                    init_uno_deck(&game.draw_pile);
                    
                    // UTILISATION DU MÉLANGE HUMAIN (Réaliste)
                    // (Assure-toi d'avoir ajouté le prototype dans card.h)
                    human_shuffle_deck(&game.draw_pile); 
                    
                    game.num_players = 1;
                    init_player(&game.players[0], 0, "Joueur Local");
                    distribute_cards(&game);
                    
                    currentState = STATE_GAME;
                }
                break;

            case STATE_GAME:
                // --- GESTION DU JEU ---
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    
                    // A. Vérifier clic sur la MAIN (Jouer une carte)
                    int clickedHandIndex = GetHoveredCardIndex(&game.players[0], assets);
                    if (clickedHandIndex != -1) {
                        try_play_card(&game, 0, clickedHandIndex);
                    }
                    else {
                        // B. Vérifier clic sur la PIOCHE (Piocher une carte)
                        Rectangle deckRect = GetDeckRect(assets);
                        if (CheckCollisionPointRec(GetMousePosition(), deckRect)) {
                            // On appelle la fonction de pioche qu'on a créée
                            player_draw_card(&game, 0);
                            printf("Action: Joueur pioche une carte.\n");
                        }
                    }
                }
                
                // Retour au menu avec Echap
                if (IsKeyPressed(KEY_ESCAPE)) {
                    currentState = STATE_MENU;
                }
                break;
        }

        // Gestion Plein écran (Fonctionne tout le temps, F11 / Alt+Enter)
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

            switch (currentState) {
                case STATE_MENU:
                    // On dessine la table vide en fond pour l'ambiance
                    if (game.draw_pile.size == 0) { 
                        // Petite astuce: si le jeu n'est pas lancé, on affiche juste un rectangle bleu
                        // ou on ne dessine rien de spécial, RenderMenu fait le travail.
                    }
                    RenderMenu(); // Affiche le titre et "Cliquez pour jouer"
                    break;

                case STATE_GAME:
                    RenderTable(&game, assets);
                    RenderHand(&game.players[0], assets);
                    
                    // HUD / Interface
                    DrawFPS(10, 10);
                    DrawText("ECHAP pour Menu", 10, 30, 20, WHITE);
                    
                    // Couleur Active
                    const char* cColors[] = {"ROUGE", "JAUNE", "VERT", "BLEU", "NOIR"};
                    const char* activeColor = (game.active_color >= 0 && game.active_color <= 4) ? cColors[game.active_color] : "---";
                    DrawText(TextFormat("Couleur Active: %s", activeColor), 10, 60, 20, WHITE);
                    break;
            }

        EndDrawing();
    }

    // --- 4. NETTOYAGE ---
    free_deck(&game.draw_pile);
    free_deck(&game.discard_pile);
    for(int i=0; i<game.num_players; i++) free_deck(&game.players[i].hand);
    UnloadAssets(assets);
    
    CloseWindow();

    return 0;
}