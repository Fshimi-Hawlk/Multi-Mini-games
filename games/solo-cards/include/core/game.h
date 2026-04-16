/**
    @file game.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Core game logic for Solitaire
*/
#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "solitaire.h"
#include "utils/types.h"
#include "utils/globals.h"
#include "utils/audio.h"

/**
    @brief Initialize solitaire game
    @param[out] game Game state to initialize
*/
void solitaire_init(SolitaireGameState_St* game);

/**
    @brief Update game logic
    @param[in,out] game Game state
    @param[in]     deltaTime Time since last frame
*/
void solitaire_update(SolitaireGameState_St* game, float deltaTime);

/**
    @brief Draw game
    @param[in]     game Game state
*/
void solitaire_draw(const SolitaireGameState_St* game);

/**
    @brief Cleanup resources
    @param[in,out] game Game state
*/
void solitaire_cleanup(SolitaireGameState_St* game);

/**
    @brief Shuffle and deal cards
    @param[in,out] game Game state
*/
void solitaire_dealCards(SolitaireGameState_St* game);

/**
    @brief Check if card is red
    @param[in]     card Card to check
    @return                    True if red, false otherwise
*/
bool solitaire_isRed(const Card_St* card);

/**
    @brief Check if move is valid
    @param[in]     card       Card to move
    @param[in]     targetPile Target pile
    @return                    True if valid, false otherwise
*/
bool solitaire_isValidMove(const Card_St* card, const Pile_St* targetPile);

/**
    @brief Check win condition
    @param[in,out] game Game state
*/
void solitaire_checkWin(SolitaireGameState_St* game);

/**
    @brief Check lose condition
    @param[in,out] game Game state
*/
void solitaire_checkLose(SolitaireGameState_St* game);

/**
    @brief Get suit color
    @param[in]     suit Suit
    @return                    Color
*/
Color solitaire_getSuitColor(Suit_Et suit);

#endif // CORE_GAME_H

