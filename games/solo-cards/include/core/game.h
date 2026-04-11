#ifndef CORE_GAME_H
#define CORE_GAME_H

#include "solitaire.h"
#include "utils/types.h"
#include "utils/globals.h"
#include "utils/audio.h"

void solitaire_init(SolitaireGameState* game);
void solitaire_update(SolitaireGameState* game, float deltaTime);
void solitaire_draw(const SolitaireGameState* game);
void solitaire_cleanup(SolitaireGameState* game);
void solitaire_dealCards(SolitaireGameState* game);
bool solitaire_isRed(const Card_St* card);
bool solitaire_isValidMove(const Card_St* card, const Pile_St* targetPile);
void solitaire_checkWin(SolitaireGameState* game);
void solitaire_checkLose(SolitaireGameState* game);
Color solitaire_getSuitColor(Suit_Et suit);

#endif
