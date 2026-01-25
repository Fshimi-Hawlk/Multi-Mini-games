#include "raylib.h"
#include <stdio.h>
#include <time.h>

#include "core/game.h"   
#include "ui/renderer.h"

int main(void)
{
    // --- 1. CONFIGURATION DE LA FENÊTRE ---
    const int initialWidth = 1280;
    const int initialHeight = 720;
    
    // On permet le redimensionnement de la fenêtre
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    
    InitWindow(initialWidth, initialHeight, "King For Four - Raylib");
    
    // Limite les FPS à 144 pour éviter de faire fondre le GPU (optionnel si VSync activé)
    SetTargetFPS(144);

    // --- 2. INITIALISATION DU JEU ---
    srand(time(NULL));
    GameAssets assets = LoadAssets();
    
    GameState game = {0}; 
    init_game_logic(&game);
    init_uno_deck(&game.draw_pile);
    shuffle_deck(&game.draw_pile);
    
    game.num_players = 1;
    init_player(&game.players[0], 0, "Joueur Local");
    distribute_cards(&game);

    // --- 3. BOUCLE PRINCIPALE ---
    while (!WindowShouldClose())
    {
        // === A. GESTION DU TEMPS (DELTA TIME) ===
        // dt contient le temps écoulé en secondes 
        float dt = GetFrameTime(); 

        // animationTimer -= dt;


        // === B. GESTION DES MODES D'ÉCRAN ===
        
        // F11 : Bascule Plein Écran Classique <-> Fenêtré
        if (IsKeyPressed(KEY_F11)) {
            // Si on est en borderless, on revient d'abord en fenêtré pour éviter les bugs
            if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) {
                ToggleBorderlessWindowed();
            }
            ToggleFullscreen();
        }

        // Alt + Entrée : Bascule Plein Écran Fenêtré (Borderless)
        if ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER)) {
            // Si on est en fullscreen exclusif, on l'enlève d'abord
            if (IsWindowState(FLAG_FULLSCREEN_MODE)) {
                ToggleFullscreen();
            }
            ToggleBorderlessWindowed();
        }


        // === C. MISE À JOUR (UPDATE) ===
        // passer 'dt' à aux fonctions d'update plus tard)


        // === D. DESSIN (DRAW) ===
        BeginDrawing();
            
            // Le fond s'adapte à la taille actuelle de la fenêtre
            ClearBackground((Color){0, 100, 0, 255}); 

            RenderTable(&game, assets);
            RenderHand(&game.players[0], assets);

            // Interface de Debug (FPS + Mode écran)
            DrawFPS(10, 10);
            
            const char* modeText = "FENÊTRÉ (F11: Full, Alt+Enter: Borderless)";
            if (IsWindowState(FLAG_FULLSCREEN_MODE)) modeText = "PLEIN ÉCRAN EXCLUSIF";
            else if (IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) modeText = "PLEIN ÉCRAN FENÊTRÉ (BORDERLESS)";
            
            DrawText(modeText, 10, 30, 20, YELLOW);

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