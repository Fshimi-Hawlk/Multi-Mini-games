#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define CARD_WIDTH 64
#define CARD_HEIGHT 96
#define NUM_CARDS 52
#define NUM_SUITS 4
#define NUM_RANKS 13

typedef enum { HEARTS, DIAMONDS, CLUBS, SPADES } Suit;
typedef enum { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING } Rank;

typedef struct {
    Rank rank;
    Suit suit;
    bool faceUp;
    Vector2 position;
    bool selected;
} Card;

typedef struct {
    Card deck[NUM_CARDS];
    Card* tableau[7][20];  // 7 colonnes, max 20 cartes
    int tableauCount[7];
    Card* foundation[4][13]; // 4 piles, max 13 cartes
    int foundationCount[4];
    Card* stock[24];
    int stockCount;
    Card* waste[24];
    int wasteCount;
    Card* draggedCards[20];
    int draggedCount;
    int dragSource;
    bool dragging;
    bool gameWon;
    int moves;
} GameState;

Texture2D texCards;
Texture2D texCardBack;
Texture2D texCardSlot;
Texture2D texBackground;

Rectangle GetCardSourceRect(Card* card) {
    if (!card->faceUp) return (Rectangle){0, 0, 0, 0};
    
    int col = card->rank - 1;
    int row = card->suit;
    
    // Sprite sheet: 13 colonnes (rangs) x 4 lignes (couleurs)
    float cardW = texCards.width / 13.0f;
    float cardH = texCards.height / 4.0f;
    
    return (Rectangle){col * cardW, row * cardH, cardW, cardH};
}

void InitDeck(GameState* game) {
    int index = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++) {
        for (int rank = 1; rank <= NUM_RANKS; rank++) {
            game->deck[index].suit = suit;
            game->deck[index].rank = rank;
            game->deck[index].faceUp = false;
            game->deck[index].selected = false;
            index++;
        }
    }
    
    // Mélanger le deck
    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = game->deck[i];
        game->deck[i] = game->deck[j];
        game->deck[j] = temp;
    }
}

void InitGame(GameState* game) {
    srand(time(NULL));
    
    game->dragging = false;
    game->draggedCount = 0;
    game->gameWon = false;
    game->moves = 0;
    
    // Réinitialiser toutes les piles
    for (int i = 0; i < 7; i++) {
        game->tableauCount[i] = 0;
        for (int j = 0; j < 20; j++) {
            game->tableau[i][j] = NULL;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        game->foundationCount[i] = 0;
        for (int j = 0; j < 13; j++) {
            game->foundation[i][j] = NULL;
        }
    }
    
    game->stockCount = 0;
    game->wasteCount = 0;
    
    InitDeck(game);
    
    // Distribuer les cartes au tableau
    int deckIndex = 0;
    for (int col = 0; col < 7; col++) {
        for (int row = 0; row <= col; row++) {
            game->tableau[col][row] = &game->deck[deckIndex];
            game->tableauCount[col]++;
            
            if (row == col) {
                game->deck[deckIndex].faceUp = true;
            }
            
            deckIndex++;
        }
    }
    
    // Le reste va dans la pioche
    while (deckIndex < NUM_CARDS) {
        game->stock[game->stockCount++] = &game->deck[deckIndex++];
    }
}

void LoadAssets() {
    texCards = LoadTexture("assets/cards.png");
    texCardBack = LoadTexture("assets/card_back.png");
    texCardSlot = LoadTexture("assets/card_slot.png");
    texBackground = LoadTexture("assets/solitaire_background.png");
}

void UnloadAssets() {
    UnloadTexture(texCards);
    UnloadTexture(texCardBack);
    UnloadTexture(texCardSlot);
    UnloadTexture(texBackground);
}

bool CanPlaceOnFoundation(Card* card, int foundationIndex, GameState* game) {
    if (!card || !card->faceUp) return false;
    
    int count = game->foundationCount[foundationIndex];
    
    if (count == 0) {
        return card->rank == ACE;
    }
    
    Card* topCard = game->foundation[foundationIndex][count - 1];
    return (card->suit == topCard->suit && card->rank == topCard->rank + 1);
}

bool CanPlaceOnTableau(Card* card, int tableauIndex, GameState* game) {
    if (!card || !card->faceUp) return false;
    
    int count = game->tableauCount[tableauIndex];
    
    if (count == 0) {
        return card->rank == KING;
    }
    
    Card* topCard = game->tableau[tableauIndex][count - 1];
    if (!topCard->faceUp) return false;
    
    bool redCard = (card->suit == HEARTS || card->suit == DIAMONDS);
    bool redTop = (topCard->suit == HEARTS || topCard->suit == DIAMONDS);
    
    return (redCard != redTop) && (card->rank == topCard->rank - 1);
}

void CheckWin(GameState* game) {
    int totalCards = 0;
    for (int i = 0; i < 4; i++) {
        totalCards += game->foundationCount[i];
    }
    if (totalCards == NUM_CARDS) {
        game->gameWon = true;
    }
}

void Update(GameState* game) {
    if (game->gameWon) {
        if (IsKeyPressed(KEY_R)) {
            InitGame(game);
        }
        return;
    }
    
    Vector2 mousePos = GetMousePosition();
    
    // Clic sur la pioche
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Rectangle stockRect = {20, 20, CARD_WIDTH, CARD_HEIGHT};
        if (CheckCollisionPointRec(mousePos, stockRect)) {
            if (game->stockCount > 0) {
                game->stock[game->stockCount - 1]->faceUp = true;
                game->waste[game->wasteCount++] = game->stock[--game->stockCount];
                game->moves++;
            } else if (game->wasteCount > 0) {
                // Recycler la défausse dans la pioche
                while (game->wasteCount > 0) {
                    game->waste[game->wasteCount - 1]->faceUp = false;
                    game->stock[game->stockCount++] = game->waste[--game->wasteCount];
                }
            }
        }
    }
    
    // Double-clic pour auto-placement en fondation
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        // Vérifier la défausse
        if (game->wasteCount > 0) {
            Rectangle wasteRect = {100, 20, CARD_WIDTH, CARD_HEIGHT};
            if (CheckCollisionPointRec(mousePos, wasteRect)) {
                Card* card = game->waste[game->wasteCount - 1];
                for (int i = 0; i < 4; i++) {
                    if (CanPlaceOnFoundation(card, i, game)) {
                        game->foundation[i][game->foundationCount[i]++] = card;
                        game->wasteCount--;
                        game->moves++;
                        CheckWin(game);
                        break;
                    }
                }
            }
        }
        
        // Vérifier le tableau
        for (int col = 0; col < 7; col++) {
            if (game->tableauCount[col] == 0) continue;
            
            int x = 20 + col * (CARD_WIDTH + 10);
            int y = 140 + (game->tableauCount[col] - 1) * 25;
            Rectangle cardRect = {x, y, CARD_WIDTH, CARD_HEIGHT};
            
            if (CheckCollisionPointRec(mousePos, cardRect)) {
                Card* card = game->tableau[col][game->tableauCount[col] - 1];
                if (card->faceUp) {
                    for (int i = 0; i < 4; i++) {
                        if (CanPlaceOnFoundation(card, i, game)) {
                            game->foundation[i][game->foundationCount[i]++] = card;
                            game->tableauCount[col]--;
                            
                            // Retourner la carte suivante
                            if (game->tableauCount[col] > 0) {
                                game->tableau[col][game->tableauCount[col] - 1]->faceUp = true;
                            }
                            game->moves++;
                            CheckWin(game);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Draw(GameState* game) {
    BeginDrawing();
    ClearBackground(DARKGREEN);
    
    // Fond
    DrawTexturePro(texBackground,
        (Rectangle){0, 0, texBackground.width, texBackground.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0}, 0.0f, WHITE);
    
    // Pioche
    if (game->stockCount > 0) {
        DrawTexturePro(texCardBack,
            (Rectangle){0, 0, texCardBack.width, texCardBack.height},
            (Rectangle){20, 20, CARD_WIDTH, CARD_HEIGHT},
            (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawTexturePro(texCardSlot,
            (Rectangle){0, 0, texCardSlot.width, texCardSlot.height},
            (Rectangle){20, 20, CARD_WIDTH, CARD_HEIGHT},
            (Vector2){0, 0}, 0.0f, WHITE);
    }
    
    // Défausse
    if (game->wasteCount > 0) {
        Card* topCard = game->waste[game->wasteCount - 1];
        Rectangle srcRect = GetCardSourceRect(topCard);
        DrawTexturePro(texCards, srcRect,
            (Rectangle){100, 20, CARD_WIDTH, CARD_HEIGHT},
            (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawTexturePro(texCardSlot,
            (Rectangle){0, 0, texCardSlot.width, texCardSlot.height},
            (Rectangle){100, 20, CARD_WIDTH, CARD_HEIGHT},
            (Vector2){0, 0}, 0.0f, WHITE);
    }
    
    // Fondations
    for (int i = 0; i < 4; i++) {
        int x = GetScreenWidth() - 300 + i * (CARD_WIDTH + 10);
        
        if (game->foundationCount[i] > 0) {
            Card* topCard = game->foundation[i][game->foundationCount[i] - 1];
            Rectangle srcRect = GetCardSourceRect(topCard);
            DrawTexturePro(texCards, srcRect,
                (Rectangle){x, 20, CARD_WIDTH, CARD_HEIGHT},
                (Vector2){0, 0}, 0.0f, WHITE);
        } else {
            DrawTexturePro(texCardSlot,
                (Rectangle){0, 0, texCardSlot.width, texCardSlot.height},
                (Rectangle){x, 20, CARD_WIDTH, CARD_HEIGHT},
                (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
    
    // Tableau
    for (int col = 0; col < 7; col++) {
        int x = 20 + col * (CARD_WIDTH + 10);
        
        if (game->tableauCount[col] == 0) {
            DrawTexturePro(texCardSlot,
                (Rectangle){0, 0, texCardSlot.width, texCardSlot.height},
                (Rectangle){x, 140, CARD_WIDTH, CARD_HEIGHT},
                (Vector2){0, 0}, 0.0f, WHITE);
        } else {
            for (int row = 0; row < game->tableauCount[col]; row++) {
                Card* card = game->tableau[col][row];
                int y = 140 + row * 25;
                
                if (card->faceUp) {
                    Rectangle srcRect = GetCardSourceRect(card);
                    DrawTexturePro(texCards, srcRect,
                        (Rectangle){x, y, CARD_WIDTH, CARD_HEIGHT},
                        (Vector2){0, 0}, 0.0f, WHITE);
                } else {
                    DrawTexturePro(texCardBack,
                        (Rectangle){0, 0, texCardBack.width, texCardBack.height},
                        (Rectangle){x, y, CARD_WIDTH, CARD_HEIGHT},
                        (Vector2){0, 0}, 0.0f, WHITE);
                }
            }
        }
    }
    
    // Interface
    DrawText(TextFormat("Moves: %d", game->moves), 10, GetScreenHeight() - 40, 25, WHITE);
    
    if (game->gameWon) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));
        DrawText("VICTORY!", GetScreenWidth() / 2 - 150, GetScreenHeight() / 2 - 60, 80, GOLD);
        DrawText(TextFormat("Completed in %d moves!", game->moves),
            GetScreenWidth() / 2 - 200, GetScreenHeight() / 2 + 40, 35, WHITE);
        DrawText("Press R to Play Again", GetScreenWidth() / 2 - 150,
            GetScreenHeight() / 2 + 100, 30, LIGHTGRAY);
    }
    
    DrawText("Click Stock to Draw | Double-click to Auto-move", 10, GetScreenHeight() - 70, 20, LIGHTGRAY);
    
    EndDrawing();
}

int main(void) {
    InitWindow(0, 0, "Solitaire");
    ToggleFullscreen();
    SetTargetFPS(60);
    
    LoadAssets();
    
    GameState game;
    InitGame(&game);
    
    while (!WindowShouldClose()) {
        Update(&game);
        Draw(&game);
    }
    
    UnloadAssets();
    CloseWindow();
    
    return 0;
}
