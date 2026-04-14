/**
    @file renderer.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Renderer for Solitaire
*/
#ifndef RENDERER_H
#define RENDERER_H

#include <raylib.h>

/**
    @brief Game assets structure
*/
typedef struct GameAssets_s {
    Texture2D cardSheet;        ///< Texture for card sprites
    Texture2D cardBack;         ///< Texture for card back
    Texture2D menuBackground;   ///< Texture for menu background
} GameAssets_St;

#include "solitaire.h"

// --- GESTION DES ASSETS ---

/**
    @brief Load game assets
    @return                  Loaded assets
*/
GameAssets_St solitaire_LoadAssets(void);

/**
    @brief Unload game assets
    @param[in] assets        Assets to unload
*/
void solitaire_UnloadAssets(GameAssets_St assets);

// --- FONCTIONS D'AFFICHAGE ---

/**
    @brief Render game
    @param[in] game          Game state
    @param[in] assets        Game assets
*/
void RenderGame(const SolitaireGameState_St *game, GameAssets_St assets);

/**
    @brief Render card
    @param[in] card          Card to render
    @param[in] assets        Game assets
    @param[in] position      Position
*/
void RenderCard(const Card_St *card, GameAssets_St assets, Vector2 position);

/**
    @brief Render pile
    @param[in] pile          Pile to render
    @param[in] assets        Game assets
*/
void RenderPile(const Pile_St *pile, GameAssets_St assets);

/**
    @brief Render menu
    @param[in] game          Game state
    @param[in] assets        Game assets
*/
void solitaire_RenderMenu(const SolitaireGameState_St *game, GameAssets_St assets);

#endif // RENDERER_H