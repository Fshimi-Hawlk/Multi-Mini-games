#include "ui/renderer.h"
#include <stdio.h>

// Dimensions de ta planche playingCards.png
#define SHEET_COLS 5
#define SHEET_ROWS 13

// --- CHARGEMENT DES RESSOURCES ---
GameAssets LoadAssets(void) {
    GameAssets assets;
    
    // Chargement de la planche de sprites (Faces)
    assets.cardSheet = LoadTexture("assets/textures/playingCards.png");

    // Chargement du dos (Back)
    assets.cardBack = LoadTexture("assets/textures/cardBack_blue5.png");
    
    // Vérifications de sécurité
    if (assets.cardSheet.id == 0) printf("ERREUR CRITIQUE: Texture playingCards.png introuvable !\n");
    if (assets.cardBack.id == 0)  printf("ERREUR CRITIQUE: Texture cardBack_blue5.png introuvable !\n");


    return assets;
}

void UnloadAssets(GameAssets assets) {
    UnloadTexture(assets.cardSheet);
    UnloadTexture(assets.cardBack);
}

// --- LOGIQUE DE DÉCOUPE ---
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

// --- AFFICHAGE DE LA TABLE (Talon + Pioche) ---

void RenderTable(GameState *g, GameAssets assets) {
    float scale = 1.0f;
    
    // Calcul de la taille cible (Destination) basé sur ta planche de cartes
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    // --- 1. DESSIN DE LA PIOCHE (Le Dos des cartes) ---
    Rectangle deckPos = { centerX - cardW - 20, centerY - (cardH/2), cardW, cardH };
    
    // On définit la zone de l'image source (toute l'image du dos)
    Rectangle sourceBack = { 0, 0, (float)assets.cardBack.width, (float)assets.cardBack.height };
    
    // On dessine le dos en l'étirant pour qu'il rentre exactement dans deckPos
    DrawTexturePro(assets.cardBack, sourceBack, deckPos, (Vector2){0,0}, 0.0f, WHITE);
    
    // (Optionnel) Un petit contour blanc pour faire ressortir la pile
    // DrawRectangleLinesEx(deckPos, 1, WHITE); 

    // --- 2. DESSIN DU TALON (La carte jouée) ---
    if (g->discard_pile.head != NULL) {
        Card topCard = g->discard_pile.head->card; 

        Rectangle source = GetCardSourceRec(topCard, assets.cardSheet);
        Rectangle dest = { centerX + 20, centerY - (cardH/2), cardW, cardH };
        
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
        
        if (topCard.value >= SKIP) {
            DrawText("EFFET!", dest.x, dest.y - 20, 20, RED);
        }
    }
}

// --- AFFICHAGE DE LA MAIN DU JOUEUR ---
void RenderHand(Player *p, GameAssets assets) {
    if (p->hand.head == NULL) return;

    float scale = 2.0f;
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;

    int padding = 45; // Espace entre les cartes
    int startX = 50;  // Marge gauche
    int startY = GetScreenHeight() - cardH - 20; // En bas de l'écran

    Node* current = p->hand.head;
    int index = 0;

    while (current != NULL) {
        // 1. Calcul de la position
        Rectangle dest = {
            startX + (index * padding),
            startY,
            cardW,
            cardH
        };

        // 2. Interaction Souris (Hover)
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, dest)) {
            dest.y -= 30; // La carte "monte" quand on passe dessus
        }

        // 3. Récupération de la texture
        Rectangle source = GetCardSourceRec(current->card, assets.cardSheet);

        // 4. Dessin de la carte
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);

        // 5. Ajout du texte d'aide (Ton code)
        if (current->card.value >= SKIP) {
            const char* text = "";
            switch(current->card.value) {
                case SKIP:      text = "PASSE"; break;
                case REVERSE:   text = "<--->"; break;
                case PLUS_TWO:  text = "+2"; break;
                case PLUS_FOUR: text = "+4"; break;
                case JOKER:     text = "COLOR"; break;
                default: break;
            }
            
            // Centrage approximatif du texte sur la carte
            int textX = dest.x + 5;
            int textY = dest.y + 5; // En haut à gauche de la carte
            
            DrawText(text, textX+2, textY+2, 20, BLACK); // Ombre noire
            DrawText(text, textX, textY, 20, BLACK);     // Texte blanc
            DrawText(text, textX, textY, 20, BLACK);     // (Doublé pour effet "gras")
        }

        // Passage à la carte suivante
        current = current->next;
        index++;
    }
}