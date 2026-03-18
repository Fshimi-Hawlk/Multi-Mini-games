/**
 * @file renderer.c
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 * @brief Implementation of UI rendering functions using Raylib.
 */

#include "ui/renderer.h"
#include <stdio.h>
#include <math.h>

/** @brief Number of columns in the card texture sheet. */
#define SHEET_COLS 5
/** @brief Number of rows in the card texture sheet. */
#define SHEET_ROWS 13
/** @brief Scale factor for rendering cards. */
#define CARD_SCALE 0.8f

/**
 * @brief Loads textures for cards from various possible paths.
 * @return The loaded assets.
 */
GameAssets LoadAssets(void) {
    GameAssets assets;
    
    // Tentative de chargement depuis plusieurs chemins possibles (Monorepo compat)
    assets.cardSheet = LoadTexture("assets/textures/playingCards.png");
    if (assets.cardSheet.id == 0) assets.cardSheet = LoadTexture("lobby/assets/textures/playingCards.png");

    assets.cardBack = LoadTexture("assets/textures/cardBack_blue5.png");
    if (assets.cardBack.id == 0) assets.cardBack = LoadTexture("lobby/assets/textures/cardBack_blue5.png");
    
    // Vérifications de sécurité
    if (assets.cardSheet.id == 0) printf("ERREUR CRITIQUE: Texture playingCards.png introuvable !\n");
    if (assets.cardBack.id == 0)  printf("ERREUR CRITIQUE: Texture cardBack_blue5.png introuvable !\n");

    return assets;
}

/**
 * @brief Frees GPU memory for textures.
 * @param assets The assets to unload.
 */
void UnloadAssets(GameAssets assets) {
    UnloadTexture(assets.cardSheet);
    UnloadTexture(assets.cardBack);
}

/**
 * @brief Calculates the source rectangle on the texture sheet for a given card.
 * @param c The card.
 * @param sheet The texture sheet.
 * @return The source rectangle.
 */
Rectangle GetCardSourceRec(Card c, Texture2D sheet) {
    float cellWidth = sheet.width / (float)SHEET_COLS;
    float cellHeight = sheet.height / (float)SHEET_ROWS;
    
    int col = 0;
    int row = 0;

    // 1. MAPPING COULEURS
    switch(c.color) {
        case CARD_RED:    col = 0; break;
        case CARD_YELLOW: col = 1; break;
        case CARD_GREEN:  col = 2; break;
        case CARD_BLUE:   col = 3; break;
        case CARD_BLACK:  col = 4; break;
    }

    // 2. MAPPING VALEURS
    switch(c.value) {
        case ZERO:  row = 12; break;
        case ONE:   row = 11; break;
        case TWO:   row = 10; break;
        case THREE: row = 9;  break;
        case FOUR:  row = 8;  break;
        case FIVE:  row = 7;  break;
        case SIX:   row = 6;  break;
        case SEVEN: row = 5;  break;
        case EIGHT: row = 4;  break;
        case NINE:  row = 3;  break;

        case PLUS_TWO: row = 2; break; // 10
        case SKIP:     row = 1; break; // Valet
        case REVERSE:  row = 0; break; // Dame
        
        case JOKER:     col = 4; row = 0; break;
        case PLUS_FOUR: col = 4; row = 1; break; // Roi Noir
        
        default: row = 12; break;
    }

    return (Rectangle){ col * cellWidth, row * cellHeight, cellWidth, cellHeight };
}

/**
 * @brief Draws the discard pile and draw pile.
 * @param g Game state.
 * @param assets Graphical assets.
 */
void RenderTable(GameState *g, GameAssets assets) {
    float scale = 1.0f;
    
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    // Background decoration
    DrawCircleGradient(centerX, centerY, 300, (Color){0, 100, 0, 100}, (Color){0, 0, 0, 0});

    // --- 1. DESSIN DE LA PIOCHE (Le Dos des cartes) ---
    Rectangle deckPos = { centerX - cardW - 20, centerY - (cardH/2), cardW, cardH };
    Rectangle sourceBack = { 0, 0, (float)assets.cardBack.width, (float)assets.cardBack.height };
    
    // Shadow
    DrawRectangleRounded((Rectangle){deckPos.x + 5, deckPos.y + 5, deckPos.width, deckPos.height}, 0.1f, 10, Fade(BLACK, 0.4f));
    DrawTexturePro(assets.cardBack, sourceBack, deckPos, (Vector2){0,0}, 0.0f, WHITE);
    DrawRectangleLinesEx(deckPos, 2, Fade(WHITE, 0.3f)); 

    // --- 2. DESSIN DU TALON (La carte jouée) ---
    if (g->discard_pile.head != NULL) {
        Card topCard = g->discard_pile.head->card; 

        Rectangle source = GetCardSourceRec(topCard, assets.cardSheet);
        Rectangle dest = { centerX + 20, centerY - (cardH/2), cardW, cardH };
        
        // Glow effect based on color
        Color glowColor = WHITE;
        switch(topCard.color) {
            case CARD_RED:    glowColor = RED; break;
            case CARD_YELLOW: glowColor = YELLOW; break;
            case CARD_GREEN:  glowColor = GREEN; break;
            case CARD_BLUE:   glowColor = BLUE; break;
            case CARD_BLACK:  glowColor = PURPLE; break;
        }
        DrawCircleGradient(dest.x + cardW/2, dest.y + cardH/2, 120, Fade(glowColor, 0.4f), (Color){0,0,0,0});

        // Shadow
        DrawRectangleRounded((Rectangle){dest.x + 5, dest.y + 5, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.4f));
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLinesEx(dest, 2, WHITE);
        
        if (topCard.value >= SKIP) {
            float pulse = (sinf(GetTime() * 5.0f) + 1.0f) * 0.5f;
            DrawText("! ACTION !", dest.x, dest.y - 30, 20, Fade(glowColor, 0.5f + pulse * 0.5f));
        }
    }
}

/**
 * @brief Draws the cards in the player's hand, including hover effects.
 * @param p Player.
 * @param assets Graphical assets.
 */
void RenderHand(Player *p, GameAssets assets) {
    if (p->hand.head == NULL) return;

    // --- CONFIGURATION ---
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * CARD_SCALE;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * CARD_SCALE;
    int padding = 45; 
    int startX = (GetScreenWidth() - (p->hand.size * padding + cardW)) / 2;
    if (startX < 50) startX = 50;
    int startY = GetScreenHeight() - cardH - 20;

    // --- PASSE 1 : DÉTECTION DE LA CARTE SURVOLÉE ---
    int hoveredIndex = GetHoveredCardIndex(p, assets);

    // --- PASSE 2 : DESSIN ---
    Node* current = p->hand.head; 
    int i = 0;

    while (current != NULL) {
        Rectangle dest = { (float)startX + (i * padding), (float)startY, cardW, cardH };

        // Animation de survol
        if (i == hoveredIndex) {
            dest.y -= 40;
            // Hover glow
            DrawRectangleRounded((Rectangle){dest.x - 5, dest.y - 5, dest.width + 10, dest.height + 10}, 0.1f, 10, Fade(GOLD, 0.5f));
        }

        // Card shadow
        DrawRectangleRounded((Rectangle){dest.x + 3, dest.y + 3, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.3f));

        Rectangle source = GetCardSourceRec(current->card, assets.cardSheet);
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);

        // Help text
        if (current->card.value >= SKIP && i == hoveredIndex) {
            const char* text = "";
            switch(current->card.value) {
                case SKIP:      text = "PASSE"; break;
                case REVERSE:   text = "REVERSE"; break;
                case PLUS_TWO:  text = "+2 CARDS"; break;
                case PLUS_FOUR: text = "+4 CARDS"; break;
                case JOKER:     text = "CHANGE COLOR"; break;
                default: break;
            }
            DrawText(text, dest.x, dest.y - 25, 20, GOLD);
        }

        current = current->next;
        i++;
    }
}

/**
 * @brief Returns the index of the card under the mouse cursor.
 */
int GetHoveredCardIndex(Player *p, GameAssets assets) {
    if (p->hand.head == NULL) return -1;

    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * CARD_SCALE;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * CARD_SCALE;
    int padding = 45; 
    int startX = (GetScreenWidth() - (p->hand.size * padding + cardW)) / 2;
    if (startX < 50) startX = 50;
    int startY = GetScreenHeight() - cardH - 20;

    Vector2 mouse = GetMousePosition();
    Node* current = p->hand.head;
    int hoveredIndex = -1;
    for (int i = 0; current; i++, current = current->next) {
        Rectangle hitBox = { (float)startX + (i * padding), (float)startY - (hoveredIndex == i ? 40 : 0), (float)padding, (float)cardH };
        // Last card has full width hitBox
        if (current->next == NULL) hitBox.width = cardW;
        
        if (CheckCollisionPointRec(mouse, hitBox)) {
            hoveredIndex = i;
        }
    }
    return hoveredIndex;
}

/**
 * @brief Calculates the rectangle for the deck on screen.
 * @param assets Assets.
 * @return The rectangle.
 */
Rectangle GetDeckRect(GameAssets assets) {
    float scale = 1.0f; 
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;
    
    return (Rectangle){ centerX - cardW - 20, centerY - (cardH/2), cardW, cardH };
}

/**
 * @brief Renders the main menu screen.
 */
void RenderMenu(void) {
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // Fond sombre transparent
    DrawRectangle(0, 0, sw, sh, (Color){20, 20, 20, 230});

    // Titre
    const char* title = "KING FOR FOUR";
    int titleSize = 80;
    int titleW = MeasureText(title, titleSize);
    DrawText(title, (sw - titleW)/2, sh/4, titleSize, GOLD);

    // Bouton Jouer
    const char* sub = "Cliquez pour JOUER";
    int subSize = 30;
    int subW = MeasureText(sub, subSize);
    
    // Animation simple de clignotement
    if ((GetTime() * 2.0) - (int)(GetTime() * 2.0) < 0.5) {
        DrawText(sub, (sw - subW)/2, sh/2, subSize, WHITE);
    } else {
        DrawText(sub, (sw - subW)/2, sh/2, subSize, LIGHTGRAY);
    }
    
    DrawText("v1.0 - Raylib", 10, sh - 30, 20, DARKGRAY);
}
