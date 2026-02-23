/**
 * @file solitaireAPI.h
 * @brief Public API for the Solitaire (Klondike) mini-game
 * @author Maxime CHAUVEAU
 * @date February 2026
 * 
 * Usage:
 *   SolitaireGame_St* game = solitaire_initGame(NULL);  // NULL = default config
 *   if (!game) { handle error }
 *   while (solitaire_isRunning(game)) solitaire_gameLoop(game);
 *   solitaire_freeGame(game);
 */

#ifndef SOLITAIRE_API_H
#define SOLITAIRE_API_H

#include "APIs/gameError.h"
#include "APIs/gameConfig.h"
#include <stdbool.h>

typedef struct SolitaireGame_St SolitaireGame_St;

SolitaireGame_St* solitaire_initGame(const GameConfig_St* config);
void solitaire_gameLoop(SolitaireGame_St* const game);
void solitaire_freeGame(SolitaireGame_St* game);
bool solitaire_isRunning(const SolitaireGame_St* game);

#endif // SOLITAIRE_API_H
