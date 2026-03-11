#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
// On inclut core/game.h pour connaître GameState et Player
#include "../core/game.h" 

// Structure pour stocker les textures chargées
typedef struct {
    Texture2D cardSheet; // Planche des faces
    Texture2D cardBack;  // Image du dos
    Font gameFont;       // 
} GameAssets;

// --- GESTION DES ASSETS ---
GameAssets LoadAssets(void);
void UnloadAssets(GameAssets assets);

// --- FONCTIONS D'AFFICHAGE (JEU) ---
void RenderTable(GameState *g, GameAssets assets);
void RenderHand(Player *p, GameAssets assets);

// --- FONCTIONS D'INTERACTION ---
// Celle-ci manquait peut-être aussi :
int GetHoveredCardIndex(Player *p, GameAssets assets);

// --- NOUVELLES FONCTIONS (Celles qui plantent) ---
// Récupère la hitbox de la pioche
Rectangle GetDeckRect(GameAssets assets);

// Affiche le menu principal
void RenderMenu(void);

#endif