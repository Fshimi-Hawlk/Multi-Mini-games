/**
    @file renderer.h
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Header file for the UI rendering of the King for Four game using Raylib.
*/
#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"
#include "core/game.h" 

#ifndef ASSET_PATH
/**
    @brief Root directory for all game assets.
*/
#define ASSET_PATH "assets/"
#endif

/**
 * @struct GameAssets_St
 * @brief Structure to store loaded graphical assets.
 */
typedef struct {
    Texture2D cardSheet; /**< Texture containing all card faces */
    Texture2D cardBack;  /**< Texture for the back of the cards */
    Font gameFont;       /**< Font used for in-game text */
} GameAssets_St;

// --- GESTION DES ASSETS ---

/**
    @brief Loads all necessary graphical assets for the game.
    @return A GameAssets_St structure containing the loaded textures and fonts.
*/
GameAssets_St kingForFour_loadAssets(void);

/**
    @brief Unloads graphical assets to free memory.

    @param[in] assets The GameAssets_St structure to unload.
*/
void kingForFour_unloadAssets(GameAssets_St assets);

// --- FONCTIONS D'AFFICHAGE (JEU) ---

/**
    @brief Renders the game table (draw and discard piles).
    @param g Pointer to the KingForFourGameState_St.
    @param assets The GameAssets_St to use for rendering.
    @param cardScalePop Additional scale for the top card (animation).
*/
void kingForFour_renderTable(KingForFourGameState_St *g, GameAssets_St assets, float cardScalePop);

/**
    @brief Renders a player's hand at the bottom of the screen.

    @param[in] p      Pointer to the player whose hand to render.
    @param[in] assets The assets to use for rendering.
*/
void kingForFour_renderHand(Player_St *p, GameAssets_St assets);

/**
    @brief Renders opponent hands (card backs) around the table.

    @param[in] g     Pointer to the game state.
    @param[in] assets The assets to use for rendering.
    @param[in] my_id  The ID of the local player to exclude from rendering.
*/
void kingForFour_renderOpponents(KingForFourGameState_St *g, GameAssets_St assets, int my_id);

// --- FONCTIONS D'INTERACTION ---

/**
    @brief Identifies which card in the player's hand is being hovered over by the mouse.
    @param p Pointer to the player.
    @param assets The GameAssets_St used for layout calculations.
    @return The index of the hovered card, or -1 if no card is hovered.
*/
int kingForFour_getHoveredCardIndex(Player_St *p, GameAssets_St assets);

/**
    @brief Gets the bounding rectangle of the draw pile.

    @param[in] assets The assets used for layout calculations.
    @return           A Rectangle representing the draw pile's hit area.
*/
Rectangle kingForFour_getDeckRect(GameAssets_St assets);

// --- FONCTIONS DE MENU ---

/**
 * @brief Renders the main menu of the game.
 */
void kingForFour_renderMenu(void);

#endif // RENDERER_H
