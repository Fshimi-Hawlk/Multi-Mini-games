#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

typedef struct GameAssets {
    Texture2D cardSheet;
    Texture2D cardBack;
    Texture2D menuBackground;
} GameAssets;

#include "solitaire.h"

// --- GESTION DES ASSETS ---
GameAssets LoadAssets(void);
void UnloadAssets(GameAssets assets);

// --- FONCTIONS D'AFFICHAGE ---
void RenderGame(const SolitaireGameState *game, GameAssets assets);
void RenderCard(const Card_St *card, GameAssets assets, Vector2 position);
void RenderPile(const Pile_St *pile, GameAssets assets);
void RenderMenu(const SolitaireGameState *game, GameAssets assets);

#endif