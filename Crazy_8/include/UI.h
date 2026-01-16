#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game.h"

// Structure pour stocker les textures chargées
typedef struct {
    Texture2D cardSheet;
    Texture2D background;
    Font gameFont;
} GameAssets;

// Init et déchargement des ressources
GameAssets LoadAssets(void);
void UnloadAssets(GameAssets assets);

// Fonctions de dessin
void RenderTable(GameState *g, GameAssets assets);
void RenderHand(Player *p, GameAssets assets);

#endif