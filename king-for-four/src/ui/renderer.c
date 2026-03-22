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
 * @brief Paths for assets, can be overridden by ASSET_PATH macro.
 */
#ifndef ASSET_PATH
    #define ASSET_PATH "assets/"
#endif

/**
 * @brief Loads textures for cards from various possible paths.
 * @return The loaded assets.
 */
GameAssets LoadAssets(void) {
    GameAssets assets;

    // Chargement via le chemin défini à la compilation (Monorepo ou Standalone)
    assets.cardSheet = LoadTexture(ASSET_PATH "textures/playingCards.png");
    assets.cardBack  = LoadTexture(ASSET_PATH "textures/cardBack_blue5.png");

    // Vérifications de sécurité
    if (assets.cardSheet.id == 0) {
        printf("ERREUR CRITIQUE: Texture playingCards.png introuvable à : %stextures/playingCards.png\n", ASSET_PATH);
    }
    if (assets.cardBack.id == 0) {
        printf("ERREUR CRITIQUE: Texture cardBack_blue5.png introuvable à : %stextures/cardBack_blue5.png\n", ASSET_PATH);
    }

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

    // --- DECORATION DE LA TABLE ---
    // Fond avec un léger dégradé circulaire
    DrawCircleGradient(centerX, centerY, 450, (Color){30, 80, 40, 255}, (Color){15, 40, 20, 255});
    DrawCircleLines(centerX, centerY, 305, Fade(GOLD, 0.3f));
    DrawCircleLines(centerX, centerY, 300, Fade(GOLD, 0.6f));

    // --- 1. DESSIN DE LA PIOCHE (Le Dos des cartes) ---
    Rectangle deckPos = { centerX - cardW - 30, centerY - (cardH/2), cardW, cardH };
    Rectangle sourceBack = { 0, 0, (float)assets.cardBack.width, (float)assets.cardBack.height };
    
    // Multi-layered shadow for depth
    for (int i=1; i<=5; i++) {
        DrawRectangleRounded((Rectangle){deckPos.x + i, deckPos.y + i, deckPos.width, deckPos.height}, 0.1f, 10, Fade(BLACK, 0.15f));
    }
    
    DrawTexturePro(assets.cardBack, sourceBack, deckPos, (Vector2){0,0}, 0.0f, WHITE);
    DrawRectangleLinesEx(deckPos, 2, Fade(WHITE, 0.5f)); 

    // --- 2. DESSIN DU TALON (La carte jouée) ---
    if (g->discard_pile.head != NULL) {
        Card topCard = g->discard_pile.head->card; 

        Rectangle source = GetCardSourceRec(topCard, assets.cardSheet);
        Rectangle dest = { centerX + 30, centerY - (cardH/2), cardW, cardH };
        
        // Glow effect based on color
        Color glowColor = WHITE;
        switch(topCard.color) {
            case CARD_RED:    glowColor = RED; break;
            case CARD_YELLOW: glowColor = YELLOW; break;
            case CARD_GREEN:  glowColor = GREEN; break;
            case CARD_BLUE:   glowColor = BLUE; break;
            case CARD_BLACK:  glowColor = PURPLE; break;
        }
        DrawCircleGradient(dest.x + cardW/2, dest.y + cardH/2, 150, Fade(glowColor, 0.3f), (Color){0,0,0,0});

        // Shadow
        for (int i=1; i<=5; i++) {
            DrawRectangleRounded((Rectangle){dest.x + i, dest.y + i, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.15f));
        }

        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
        DrawRectangleLinesEx(dest, 3, WHITE);
        
        if (topCard.value >= SKIP) {
            float pulse = (sinf(GetTime() * 6.0f) + 1.0f) * 0.5f;
            const char* actionText = "!! ACTION !!";
            int tw = MeasureText(actionText, 25);
            DrawText(actionText, dest.x + cardW/2 - tw/2, dest.y - 40, 25, Fade(glowColor, 0.4f + pulse * 0.6f));
        }
    }

    // --- 3. DESSIN DES MAINS ADVERSES (Dos des cartes) ---
    // Positions relatives pour les 3 autres joueurs (Top, Left, Right)
    // On suppose que le joueur local est à l'index my_internal_id (voir king_client_module.c)
    
    // Joueur Haut
    float opponentCardW = cardW * 0.6f;
    float opponentCardH = cardH * 0.6f;
    
    // Exemple : Dessiner quelques dos de cartes en haut pour l'ambiance
    // (Une implémentation complète utiliserait g->players[i].hand.size)
    for (int i = 0; i < 4; i++) {
        if (g->players[i].hand.size > 0) {
            // Calcul de position selon l'index relatif au joueur local
            // (Ici on fait un affichage générique pour le test)
            Vector2 pos = {0};
            float angle = 0;
            
            if (i == 1) { pos = (Vector2){ 100, centerY }; angle = 90; } // Gauche
            else if (i == 2) { pos = (Vector2){ centerX, 100 }; angle = 0; } // Haut
            else if (i == 3) { pos = (Vector2){ GetScreenWidth() - 100, centerY }; angle = -90; } // Droite
            else continue; // Local player
            
            int count = g->players[i].hand.size;
            if (count > 10) count = 10; // Limite visuelle
            
            for (int j = 0; j < count; j++) {
                Rectangle dest = { pos.x + (angle == 0 ? (j-count/2.0f)*20 : 0), 
                                   pos.y + (angle != 0 ? (j-count/2.0f)*20 : 0), 
                                   opponentCardW, opponentCardH };
                DrawTexturePro(assets.cardBack, sourceBack, dest, (Vector2){opponentCardW/2, opponentCardH/2}, angle, WHITE);
            }
            DrawText(TextFormat("P%d: %d", i, g->players[i].hand.size), pos.x - 20, pos.y + (angle == 0 ? 50 : 80), 20, BLACK);
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
    int padding = 55; 
    int startX = (GetScreenWidth() - (p->hand.size * padding + (cardW-padding))) / 2;
    if (startX < 50) startX = 50;
    int startY = GetScreenHeight() - cardH - 30;

    // --- PASSE 1 : DÉTECTION DE LA CARTE SURVOLÉE ---
    int hoveredIndex = GetHoveredCardIndex(p, assets);

    // --- PASSE 2 : DESSIN ---
    Node* current = p->hand.head; 
    int i = 0;

    while (current != NULL) {
        Rectangle dest = { (float)startX + (i * padding), (float)startY, cardW, cardH };

        // Animation de survol
        if (i == hoveredIndex) {
            dest.y -= 50;
            // Hover glow
            DrawRectangleRounded((Rectangle){dest.x - 7, dest.y - 7, dest.width + 14, dest.height + 14}, 0.15f, 10, Fade(GOLD, 0.4f));
            DrawRectangleRoundedLines((Rectangle){dest.x - 7, dest.y - 7, dest.width + 14, dest.height + 14}, 0.15f, 10, GOLD);
        }

        // Card shadow with depth
        DrawRectangleRounded((Rectangle){dest.x + 4, dest.y + 4, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.3f));

        Rectangle source = GetCardSourceRec(current->card, assets.cardSheet);
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
        
        if (i == hoveredIndex) {
            DrawRectangleLinesEx(dest, 3, WHITE);
        } else {
            DrawRectangleLinesEx(dest, 1, Fade(WHITE, 0.3f));
        }

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
    int padding = 55; 
    int startX = (GetScreenWidth() - (p->hand.size * padding + (cardW-padding))) / 2;
    if (startX < 50) startX = 50;
    int startY = GetScreenHeight() - cardH - 30;

    Vector2 mouse = GetMousePosition();
    Node* current = p->hand.head;
    int hoveredIndex = -1;
    for (int i = 0; current; i++, current = current->next) {
        Rectangle hitBox = { (float)startX + (i * padding), (float)startY - 40, (float)padding, (float)cardH + 40 };
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
