/**
    @file solitaire.h
    @author Maxime CHAUVEAU
    @date 2026-02-01
    @date 2026-04-14
    @brief Main header for Solitaire (Klondike)
*/
#ifndef SOLITAIRE_SOLITAIRE_H
#define SOLITAIRE_SOLITAIRE_H

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

// Game constants
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
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

/**
    @brief Card regions for sprite sheet
*/
typedef struct {
    const char* filename;   ///< Name of the file
    int x;                  ///< X position
    int y;                  ///< Y position
    int width;              ///< Width
    int height;             ///< Height
} CardRegion_St;

/**
    @brief Card suits
*/
typedef enum {
    SUIT_HEARTS = 0,        ///< Hearts suit
    SUIT_DIAMONDS,          ///< Diamonds suit
    SUIT_CLUBS,             ///< Clubs suit
    SUIT_SPADES             ///< Spades suit
} Suit_Et;

/**
    @brief Card ranks
*/
typedef enum {
    RANK_ACE = 1,           ///< Ace
    RANK_TWO,               ///< 2
    RANK_THREE,             ///< 3
    RANK_FOUR,              ///< 4
    RANK_FIVE,              ///< 5
    RANK_SIX,               ///< 6
    RANK_SEVEN,             ///< 7
    RANK_EIGHT,             ///< 8
    RANK_NINE,              ///< 9
    RANK_TEN,               ///< 10
    RANK_JACK,              ///< Jack
    RANK_QUEEN,             ///< Queen
    RANK_KING               ///< King
} Rank_Et;

/**
    @brief Card structure
*/
typedef struct {
    Suit_Et suit;           ///< Card suit
    Rank_Et rank;           ///< Card rank
    bool isFaceUp;          ///< Is the card face up?
    Vector2 position;       ///< Card position
    int pileIndex;          ///< Which pile it belongs to
    int cardIndex;          ///< Position in that pile
} Card_St;

/**
    @brief Pile types
*/
typedef enum {
    PILE_STOCK,             ///< Draw pile
    PILE_WASTE,             ///< Discard pile
    PILE_TABLEAU,           ///< 7 main piles
    PILE_FOUNDATION         ///< 4 foundation piles
} PileType_Et;

/**
    @brief Game pile
*/
typedef struct {
    Card_St* cards[NUM_CARDS];  ///< Cards in the pile
    int count;                  ///< Number of cards
    PileType_Et type;           ///< Pile type
    Vector2 position;           ///< Pile position
    Suit_Et expectedSuit;       ///< For PILE_FOUNDATION : suit attendu (pré-assigné à l'init)
} Pile_St;

/**
    @brief Dragging state
*/
typedef struct {
    bool isDragging;            ///< Is dragging?
    Card_St* cards[NUM_CARDS];  ///< Cards being dragged
    int count;                  ///< Number of cards
    Vector2 offset;             ///< Offset from mouse
    Pile_St* sourcePile;        ///< Source pile
    int sourceIndex;            ///< Source index
} DragState_St;

/**
    @brief Forward declaration for the game assets structure.
*/
typedef struct GameAssets_s GameAssets_St;

/**
    @brief Game state
*/
typedef struct {
    Card_St deck[NUM_CARDS];                    ///< All cards in the game
    
    Pile_St stock;                              ///< Draw pile
    Pile_St waste;                              ///< Discard pile
    Pile_St tableau[NUM_TABLEAU_PILES];        ///< 7 main piles
    Pile_St foundation[NUM_FOUNDATION_PILES];  ///< 4 foundation piles

    DragState_St dragState;                     ///< Current drag state
    
    int score;                                  ///< Game score
    int moves;                                  ///< Number of moves
    float gameTime;                             ///< Time elapsed
    bool isWon;                                 ///< Is game won?
    bool isLost;                                ///< Is game lost?
    
    Card_St* selectedCard;                      ///< Currently selected card
    
    GameAssets_St* assets;                      ///< Game assets
} SolitaireGameState_St;

// Function declarations

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
    @brief Move card(s) between piles
    @param[in,out] game     Game state
    @param[in,out] source   Source pile
    @param[in,out] target   Target pile
    @param[in]     numCards Number of cards to move
*/
void solitaire_moveCards(SolitaireGameState_St* game, Pile_St* source, Pile_St* target, int numCards);

/**
    @brief Draw card from stock
    @param[in,out] game Game state
*/
void solitaire_drawFromStock(SolitaireGameState_St* game);

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
    @brief Get card name string
    @param[in]     card Card
    @return                    Card name
*/
const char* solitaire_getCardName(const Card_St* card);

/**
    @brief Get suit color
    @param[in]     suit Suit
    @return                    Color
*/
Color solitaire_getSuitColor(Suit_Et suit);

/**
    @brief Draw card
    @param[in]     card       Card to draw
    @param[in]     position   Position
    @param[in]     isSelected Is selected?
*/
void solitaire_drawCard(const Card_St* card, Vector2 position, bool isSelected);

/**
    @brief Draw pile
    @param[in]     pile Pile to draw
*/
void solitaire_drawPile(const Pile_St* pile);

#endif // SOLITAIRE_SOLITAIRE_H
