/**
 * @file solitaire.h
 * @brief Main header for Solitaire (Klondike)
 * @author Maxime CHAUVEAU
 * @date February 2026
 */

#ifndef SOLITAIRE_SOLITAIRE_H
#define SOLITAIRE_SOLITAIRE_H

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

// Game constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGET_FPS 60

#define NUM_SUITS 4
#define NUM_RANKS 13
#define NUM_CARDS 52

#define NUM_TABLEAU_PILES 7
#define NUM_FOUNDATION_PILES 4

#define CARD_WIDTH 80
#define CARD_HEIGHT 110
#define CARD_SPACING 10
#define CARD_OFFSET_Y 25
#define WASTE_ZONE_WIDTH 130

// Card regions for sprite sheet
typedef struct {
    const char* filename;
    int x, y, width, height;
} CardRegion;

// Card suits
typedef enum {
    SUIT_HEARTS = 0,
    SUIT_DIAMONDS,
    SUIT_CLUBS,
    SUIT_SPADES
} Suit_Et;

// Card ranks
typedef enum {
    RANK_ACE = 1,
    RANK_TWO,
    RANK_THREE,
    RANK_FOUR,
    RANK_FIVE,
    RANK_SIX,
    RANK_SEVEN,
    RANK_EIGHT,
    RANK_NINE,
    RANK_TEN,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING
} Rank_Et;

// Card structure
typedef struct {
    Suit_Et suit;
    Rank_Et rank;
    bool isFaceUp;
    Vector2 position;
    int pileIndex;     // Which pile it belongs to
    int cardIndex;     // Position in that pile
} Card_St;

// Pile types
typedef enum {
    PILE_STOCK,        // Draw pile
    PILE_WASTE,        // Discard pile
    PILE_TABLEAU,      // 7 main piles
    PILE_FOUNDATION    // 4 foundation piles
} PileType_Et;

// Game pile
typedef struct {
    Card_St* cards[NUM_CARDS];
    int count;
    PileType_Et type;
    Vector2 position;
} Pile_St;

// Dragging state
typedef struct {
    bool isDragging;
    Card_St* cards[NUM_CARDS];
    int count;
    Vector2 offset;
    Pile_St* sourcePile;
    int sourceIndex;
} DragState_St;

// Game state
typedef struct {
    Card_St deck[NUM_CARDS];
    
    Pile_St stock;                              // Draw pile
    Pile_St waste;                              // Discard pile
    Pile_St tableau[NUM_TABLEAU_PILES];        // 7 main piles
    Pile_St foundation[NUM_FOUNDATION_PILES];  // 4 foundation piles

    DragState_St dragState;
    
    int score;
    int moves;
    float gameTime;
    bool isWon;
    bool isLost;
    
    Card_St* selectedCard;
    
    struct GameAssets* assets;
} SolitaireGameState;

// Function declarations

/** @brief Initialize solitaire game */
void solitaire_init(SolitaireGameState* game);

/** @brief Update game logic */
void solitaire_update(SolitaireGameState* game, float deltaTime);

/** @brief Draw game */
void solitaire_draw(const SolitaireGameState* game);

/** @brief Cleanup resources */
void solitaire_cleanup(SolitaireGameState* game);

/** @brief Shuffle and deal cards */
void solitaire_dealCards(SolitaireGameState* game);

/** @brief Check if card is red */
bool solitaire_isRed(const Card_St* card);

/** @brief Check if move is valid */
bool solitaire_isValidMove(const Card_St* card, const Pile_St* targetPile, const SolitaireGameState* game);

/** @brief Move card(s) between piles */
void solitaire_moveCards(SolitaireGameState* game, Pile_St* source, Pile_St* target, int numCards);

/** @brief Draw card from stock */
void solitaire_drawFromStock(SolitaireGameState* game);

/** @brief Check win condition */
void solitaire_checkWin(SolitaireGameState* game);

/** @brief Check lose condition */
void solitaire_checkLose(SolitaireGameState* game);

/** @brief Get card name string */
const char* solitaire_getCardName(const Card_St* card);

/** @brief Get suit color */
Color solitaire_getSuitColor(Suit_Et suit);

/** @brief Draw card */
void solitaire_drawCard(const Card_St* card, Vector2 position, bool isSelected);

/** @brief Draw pile */
void solitaire_drawPile(const Pile_St* pile);

#endif // SOLITAIRE_SOLITAIRE_H
