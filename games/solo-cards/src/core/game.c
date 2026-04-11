/**
 * @file game.c
 * @brief Core Solitaire game logic
 * @author Maxime CHAUVEAU
 * @date February 2026
 */

#include "core/game.h"
#include "ui/renderer.h"
#include "solitaire.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#include "utils/audio.h"


void solitaire_init(SolitaireGameState* game) {
    /* FIX: free existing assets before memset wipes the pointer.
     * When called via KEY_N "new game", game->assets is already allocated
     * and textures are GPU-resident. memset would silently lose the pointer,
     * leaking both the heap block and the GPU textures on every new game. */
    if (game->assets) {
        solitaire_UnloadAssets(*game->assets);
        free(game->assets);
        game->assets = NULL;
    }

    memset(game, 0, sizeof(SolitaireGameState));

    srand((unsigned int)time(NULL));

    game->assets = malloc(sizeof(GameAssets));
    if (game->assets) {
        *game->assets = solitaire_LoadAssets();
    }

    int cardIdx = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++) {
        for (int rank = 1; rank <= NUM_RANKS; rank++) {
            game->deck[cardIdx].suit = (Suit_Et)suit;
            game->deck[cardIdx].rank = (Rank_Et)rank;
            game->deck[cardIdx].isFaceUp = false;
            cardIdx++;
        }
    }

    // Positionner la pioche (stock) en haut à gauche
    game->stock.position = (Vector2){20, 20};
    game->waste.position = (Vector2){20 + CARD_WIDTH + CARD_SPACING, 20};

    // Positionner les 4 piles de fondation en haut à droite
    Suit_Et foundationSuits[NUM_FOUNDATION_PILES] = {
        SUIT_HEARTS, SUIT_DIAMONDS, SUIT_CLUBS, SUIT_SPADES
    };
    for (int i = 0; i < NUM_FOUNDATION_PILES; i++) {
        game->foundation[i].position = (Vector2){
            SCREEN_WIDTH - (NUM_FOUNDATION_PILES - i) * (CARD_WIDTH + CARD_SPACING),
            20
        };
        game->foundation[i].type = PILE_FOUNDATION;
        game->foundation[i].expectedSuit = foundationSuits[i];
    }

    // Positionner les 7 piles du tableau en bas
    for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
        game->tableau[i].position = (Vector2){
            20 + i * (CARD_WIDTH + CARD_SPACING),
            20 + CARD_HEIGHT + CARD_SPACING * 2
        };
        game->tableau[i].type = PILE_TABLEAU;
    }

    game->stock.type = PILE_STOCK;
    game->waste.type = PILE_WASTE;

    // Deal cards
    solitaire_dealCards(game);
}

void solitaire_dealCards(SolitaireGameState* game) {
    // Mélanger le paquet avec l'algorithme Fisher-Yates
    // On parcourt les cartes de la fin vers le début
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Choisir une carte aléatoire
        Card_St temp = game->deck[i];
        game->deck[i] = game->deck[j];
        game->deck[j] = temp;
    }

    int cardIdx = 0;

    // Distribuer les cartes dans le tableau (1 carte pour la première pile, 2 pour la deuxième, etc.)
    for (int pile = 0; pile < NUM_TABLEAU_PILES; pile++) {
        for (int card = 0; card <= pile; card++) {
            game->tableau[pile].cards[game->tableau[pile].count] = &game->deck[cardIdx];
            game->deck[cardIdx].pileIndex = pile;
            game->deck[cardIdx].cardIndex = game->tableau[pile].count;

            // Seule la carte du dessus est retournée face visible
            if (card == pile) {
                game->deck[cardIdx].isFaceUp = true;
            }

            game->tableau[pile].count++;
            cardIdx++;
        }
    }

    // Le reste des cartes va dans la pioche
    while (cardIdx < NUM_CARDS) {
        game->stock.cards[game->stock.count] = &game->deck[cardIdx];
        game->deck[cardIdx].isFaceUp = false;  // Les cartes de la pioche sont cachées
        game->stock.count++;
        cardIdx++;
    }
}

bool solitaire_isRed(const Card_St* card) {
    return card->suit == SUIT_HEARTS || card->suit == SUIT_DIAMONDS;
}

bool solitaire_isValidMove(const Card_St* card, const Pile_St* targetPile) {
    if (targetPile->type == PILE_FOUNDATION) {
        if (targetPile->count == 0) {
            return card->rank == RANK_ACE && card->suit == targetPile->expectedSuit;
        } else {
            Card_St* topCard = targetPile->cards[targetPile->count - 1];
            return card->suit == topCard->suit && card->rank == topCard->rank + 1;
        }
    } else if (targetPile->type == PILE_TABLEAU) {
        if (targetPile->count == 0) {
            return card->rank == RANK_KING;
        } else {
            Card_St* topCard = targetPile->cards[targetPile->count - 1];
            if (!topCard->isFaceUp) return false;
            return solitaire_isRed(card) != solitaire_isRed(topCard) && 
                   card->rank == topCard->rank - 1;
        }
    }
    return false;
}

void gestionStock(SolitaireGameState* game) {
    if (game->stock.count > 0) {
        // Tirer une carte de la pioche et la placer dans la défausse
        Card_St* card = game->stock.cards[game->stock.count - 1];
        game->stock.count--;
        
        card->isFaceUp = true;  // Retourner la carte face visible
        game->waste.cards[game->waste.count] = card;
        game->waste.count++;
    } else if (game->waste.count > 0) {
        // Si la pioche est vide, recycler toutes les cartes de la défausse
        while (game->waste.count > 0) {
            Card_St* card = game->waste.cards[game->waste.count - 1];
            game->waste.count--;
            
            card->isFaceUp = false;  // Remettre les cartes face cachée
            game->stock.cards[game->stock.count] = card;
            game->stock.count++;
        }
    }
}

void solitaire_update(SolitaireGameState* game, float deltaTime) {
    if (!game->isWon) {
        game->gameTime += deltaTime;
    }
    
    Vector2 mousePos = GetMousePosition();
    
    // Handle stock/waste clicks
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle stockRect = {game->stock.position.x, game->stock.position.y, 
                              CARD_WIDTH, CARD_HEIGHT};
        
        if (game->stock.count > 0) {
            if (CheckCollisionPointRec(mousePos, stockRect)) {
                gestionStock(game);
                PlaySound(sound_cardTurn);

            }
        } else {
            Rectangle recycleRect = {game->stock.position.x, game->stock.position.y, 
                                   CARD_WIDTH, CARD_HEIGHT};
            if (CheckCollisionPointRec(mousePos, recycleRect)) {
                gestionStock(game);
                PlaySound(sound_cardTurn);

            }
        }
    }
    
    // Simple card selection (placeholder for full drag-drop)
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !game->dragState.isDragging) {
        bool cardClicked = false;
        
        // Check waste pile FIRST (higher priority)
        if (game->waste.count > 0) {
            int spread = 25;
            int maxVisible = 3;
            int start = game->waste.count > maxVisible ? game->waste.count - maxVisible : 0;
            float lastCardX = game->waste.position.x + (game->waste.count - 1 - start) * spread;
            
            Rectangle wasteRect = {lastCardX, game->waste.position.y,
                                  CARD_WIDTH, CARD_HEIGHT};
            
            if (CheckCollisionPointRec(mousePos, wasteRect)) {
                Card_St* card = game->waste.cards[game->waste.count - 1];
                game->selectedCard = card;
                game->dragState.isDragging = true;
                game->dragState.sourcePile = &game->waste;
                game->dragState.sourceIndex = game->waste.count - 1;
                game->dragState.offset = (Vector2){CARD_WIDTH / 2.0f, CARD_HEIGHT / 2.0f};
                game->dragState.cards[0] = card;
                game->dragState.count = 1;
                cardClicked = true;
                
                PlaySound(sound_cardTurn);

            }
        }
        
        // Check tableau piles
        if (!cardClicked) {
            for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
                if (game->tableau[i].count > 0) {
                    for (int j = game->tableau[i].count - 1; j >= 0; j--) {
                        Card_St* card = game->tableau[i].cards[j];
                        if (card->isFaceUp) {
                            Vector2 cardPos = {
                                game->tableau[i].position.x,
                                game->tableau[i].position.y + j * CARD_OFFSET_Y
                            };
                            Rectangle cardRect = {cardPos.x, cardPos.y, CARD_WIDTH, CARD_HEIGHT};
                            
                            if (CheckCollisionPointRec(mousePos, cardRect)) {
                                game->selectedCard = card;
                                game->dragState.isDragging = true;
                                game->dragState.sourcePile = &game->tableau[i];
                                game->dragState.sourceIndex = j;
                                game->dragState.offset = (Vector2){CARD_WIDTH / 2.0f, CARD_HEIGHT / 2.0f};
                                
                                // Add all cards above the clicked card
                                game->dragState.count = 0;
                                for (int k = j; k < game->tableau[i].count; k++) {
                                    game->dragState.cards[game->dragState.count++] = game->tableau[i].cards[k];
                                    PlaySound(sound_cardTurn);
                                }
                                cardClicked = true;
                                break;

                            }
                        }
                    }
                }
                if (cardClicked) break;
            }
        }


    }
    
    // Handle drag release
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && game->dragState.isDragging) {
        bool moved = false;
        int sourceIdx = game->dragState.sourceIndex;
        int dragCount = game->dragState.count;
        
        // Try foundation piles
        for (int i = 0; i < NUM_FOUNDATION_PILES; i++) {
            Rectangle foundRect = {game->foundation[i].position.x, 
                                  game->foundation[i].position.y,
                                  CARD_WIDTH, CARD_HEIGHT};
            
            if (CheckCollisionPointRec(mousePos, foundRect) && dragCount == 1) {
                if (solitaire_isValidMove(game->dragState.cards[0], &game->foundation[i])) {
                    // Move to foundation
                    Pile_St* srcPile = game->dragState.sourcePile;
                    for (int k = sourceIdx; k < srcPile->count - dragCount; k++) {
                        srcPile->cards[k] = srcPile->cards[k + dragCount];
                    }
                    srcPile->count -= dragCount;
                    
                    game->foundation[i].cards[game->foundation[i].count++] = 
                        game->dragState.cards[0];
                    
                    // Flip card if needed
                    if (srcPile->count > 0 && srcPile->type == PILE_TABLEAU) {
                        Card_St* topCard = srcPile->cards[srcPile->count - 1];
                        topCard->isFaceUp = true;
                    }
                    
                    game->score += 10;
                    /* FIX: PlaySound moved inside the successful-move block — it was
                     * previously after the closing brace of the outer if, causing it
                     * to play NUM_FOUNDATION_PILES times on every drag release. */
                    PlaySound(sound_cardTurn);
                    moved = true;
                    break;
                }
                
            }
        }
        
        // Try tableau piles
        if (!moved) {
            for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
                Rectangle tabRect = {game->tableau[i].position.x,
                                    game->tableau[i].position.y,
                                    CARD_WIDTH, CARD_HEIGHT + game->tableau[i].count * CARD_OFFSET_Y};
                
                if (CheckCollisionPointRec(mousePos, tabRect)) {
                    if (solitaire_isValidMove(game->dragState.cards[0], &game->tableau[i])) {
                        // Move to tableau
                        Pile_St* srcPile = game->dragState.sourcePile;
                        for (int k = sourceIdx; k < srcPile->count - dragCount; k++) {
                            srcPile->cards[k] = srcPile->cards[k + dragCount];
                        }
                        srcPile->count -= dragCount;
                        
                        for (int j = 0; j < game->dragState.count; j++) {
                            game->tableau[i].cards[game->tableau[i].count++] = 
                                game->dragState.cards[j];
                        }
                        
                        // Flip card if needed
                        if (srcPile->count > 0 && srcPile->type == PILE_TABLEAU) {
                            Card_St* topCard = srcPile->cards[srcPile->count - 1];
                            topCard->isFaceUp = true;
                        }
                        
                        game->score += 5;
                        moved = true;
                        break;
                    }
                }
            }
        }
        
        game->dragState.isDragging = false;
        game->dragState.count = 0;
        game->selectedCard = NULL;
    }
    
    // Check win
    solitaire_checkWin(game);
    
    // Check loss every 2 minutes
    if ((int)(game->gameTime) > 0 && (int)(game->gameTime) % 120 == 0 && (int)(game->gameTime - deltaTime) % 120 != 0) {
        solitaire_checkLose(game);
    }
    
    // New game on N key
    if (IsKeyPressed(KEY_N)) {
        solitaire_init(game);
    }
}

void solitaire_checkWin(SolitaireGameState* game) {
    int totalCards = 0;
    for (int i = 0; i < NUM_FOUNDATION_PILES; i++) {
        totalCards += game->foundation[i].count;
    }
    
    if (totalCards == NUM_CARDS) {
        game->isWon = true;
    }
}

void solitaire_checkLose(SolitaireGameState* game) {
    if (game->isWon) return;

    /* FIX: if the stock still has cards the player can draw, it is never a
     * loss — we do not know what those cards will unlock. Only evaluate
     * movability when both stock and waste are exhausted (no more draws). */
    if (game->stock.count > 0) return;

    if (game->stock.count == 0 && game->waste.count == 0) {
        game->isLost = true;
        return;
    }

    /* Check whether any visible tableau top card has a legal move. */
    for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
        if (game->tableau[i].count > 0) {
            Card_St* topCard = game->tableau[i].cards[game->tableau[i].count - 1];
            if (topCard->isFaceUp) {
                for (int j = 0; j < NUM_FOUNDATION_PILES; j++) {
                    if (solitaire_isValidMove(topCard, &game->foundation[j])) {
                        return;
                    }
                }
                for (int j = 0; j < NUM_TABLEAU_PILES; j++) {
                    if (j != i && solitaire_isValidMove(topCard, &game->tableau[j])) {
                        return;
                    }
                }
            }
        }
    }

    /* FIX: also test the top card of the waste pile — it is playable and was
     * previously ignored, causing premature loss detection. */
    if (game->waste.count > 0) {
        Card_St* wasteTop = game->waste.cards[game->waste.count - 1];
        for (int i = 0; i < NUM_FOUNDATION_PILES; i++) {
            if (solitaire_isValidMove(wasteTop, &game->foundation[i])) {
                return;
            }
        }
        for (int i = 0; i < NUM_TABLEAU_PILES; i++) {
            if (solitaire_isValidMove(wasteTop, &game->tableau[i])) {
                return;
            }
        }
    }

    game->isLost = true;
}

Color solitaire_getSuitColor(Suit_Et suit) {
    switch (suit) {
        case SUIT_HEARTS:
        case SUIT_DIAMONDS:
            return RED;
        case SUIT_CLUBS:
        case SUIT_SPADES:
            return BLACK;
        default:
            return BLACK;
    }
}

void solitaire_cleanup(SolitaireGameState* game) {
    if (game->assets) {
        solitaire_UnloadAssets(*game->assets);
        free(game->assets);
        game->assets = NULL;
    }
}

void solitaire_draw(const SolitaireGameState* game) {
    if (game->assets) {
        RenderGame(game, *game->assets);
        solitaire_RenderMenu(game, *game->assets);
    }
}