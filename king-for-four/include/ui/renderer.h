/**
 * @file renderer.h
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Header file for the UI rendering of the King for Four game using Raylib.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "../core/game.h" 

/**
 * @struct GameAssets
 * @brief Structure to store loaded graphical assets.
 */
typedef struct {
    Texture2D cardSheet; /**< Texture containing all card faces */
    Texture2D cardBack;  /**< Texture for the back of the cards */
    Font gameFont;       /**< Font used for in-game text */
} GameAssets;

// --- GESTION DES ASSETS ---

/**
 * @brief Loads all necessary graphical assets for the game.
 * @return A GameAssets structure containing the loaded textures and fonts.
 */
GameAssets LoadAssets(void);

/**
 * @brief Unloads graphical assets to free memory.
 * @param assets The GameAssets structure to unload.
 */
void UnloadAssets(GameAssets assets);

// --- FONCTIONS D'AFFICHAGE (JEU) ---

/**
 * @brief Renders the game table (draw and discard piles).
 * @param g Pointer to the GameState.
 * @param assets The GameAssets to use for rendering.
 * @param cardScalePop Additional scale for the top card (animation).
 */
void RenderTable(GameState *g, GameAssets assets, float cardScalePop);

/**
 * @brief Renders a player's hand at the bottom of the screen.
 * @param p Pointer to the player whose hand to render.
 * @param assets The GameAssets to use for rendering.
 */
void RenderHand(Player *p, GameAssets assets);

/**
 * @brief Renders opponent hands (card backs) around the table.
 */
void RenderOpponents(GameState *g, GameAssets assets, int my_id);

// --- FONCTIONS D'INTERACTION ---

/**
 * @brief Identifies which card in the player's hand is being hovered over by the mouse.
 * @param p Pointer to the player.
 * @param assets The GameAssets used for layout calculations.
 * @return The index of the hovered card, or -1 if no card is hovered.
 */
int GetHoveredCardIndex(Player *p, GameAssets assets);

/**
 * @brief Gets the bounding rectangle of the draw pile.
 * @param assets The GameAssets used for layout calculations.
 * @return A Rectangle representing the draw pile's hit area.
 */
Rectangle GetDeckRect(GameAssets assets);

// --- FONCTIONS DE MENU ---

/**
 * @brief Renders the main menu of the game.
 */
void RenderMenu(void);

#endif
