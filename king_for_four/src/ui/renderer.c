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
    
    // Un petit contour blanc pour faire ressortir la pile
    DrawRectangleLinesEx(deckPos, 1, WHITE); 

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

    // --- CONFIGURATION ---
    float scale = 1.0f; // Assure-toi que c'est le même scale que dans RenderTable si tu veux
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    int padding = 45; 
    int startX = 50;  
    int startY = GetScreenHeight() - cardH - 20;

    // --- PASSE 1 : DÉTECTION DE LA CARTE SURVOLÉE ---
    // On doit trouver L'UNIQUE carte qui doit réagir (la plus haute dans la pile visuelle)
    int hoveredIndex = -1; // -1 signifie "aucune carte"
    
    Node* current = p->hand.head;
    int i = 0;
    Vector2 mouse = GetMousePosition();

    while (current != NULL) {
        // On calcule la zone de collision "théorique" (position au repos)
        Rectangle hitBox = {
            startX + (i * padding),
            startY,
            cardW,
            cardH
        };

        // Si la souris touche cette carte, on note son index.
        // Comme on avance de gauche à droite (i augmente), si la souris est 
        // sur la superposition de la carte 1 et 2, la carte 2 écrasera la carte 1.
        // C'est exactement ce qu'on veut !
        if (CheckCollisionPointRec(mouse, hitBox)) {
            hoveredIndex = i;
        }

        current = current->next;
        i++;
    }

    // --- PASSE 2 : DESSIN ---
    current = p->hand.head; // On rembobine au début de la liste
    i = 0;

    while (current != NULL) {
        Rectangle dest = { startX + (i * padding), startY, cardW, cardH };

        // Animation de survol
        if (i == hoveredIndex) {
            dest.y -= 30; 
        }

        Rectangle source = GetCardSourceRec(current->card, assets.cardSheet);
        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);

        // 5. Texte d'aide (UNIQUEMENT AU SURVOL)
        if (current->card.value >= SKIP && i == hoveredIndex) {
            const char* text = "";
            switch(current->card.value) {
                case SKIP:      text = "PASSE"; break;
                case REVERSE:   text = "<--->"; break;
                case PLUS_TWO:  text = "+2"; break;
                case PLUS_FOUR: text = "+4"; break;
                case JOKER:     text = "COLOR"; break;
                default: break;
            }
            
            // 150 pour que ca s'affiche a cote de la carte 
            int textX = dest.x + 150; 
            int textY = dest.y + 5;
            
            DrawText(text, textX+2, textY+2, 20, BLACK); // Ombre
            DrawText(text, textX, textY, 20, WHITE);     // Texte (Blanc pour contraste sur carte)
            DrawText(text, textX, textY, 20, WHITE);     // Gras
        }

        current = current->next;
        i++;
    }
}
int GetHoveredCardIndex(Player *p, GameAssets assets) {
    if (p->hand.head == NULL) return -1;

    // Doit être IDENTIQUE à RenderHand
    float scale = 1.0f; 
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    int padding = 45; 
    int startX = 50;  
    int startY = GetScreenHeight() - cardH - 20;

    int hoveredIndex = -1;
    Node* current = p->hand.head;
    int i = 0;
    Vector2 mouse = GetMousePosition();

    while (current != NULL) {
        Rectangle hitBox = { startX + (i * padding), startY, cardW, cardH };
        
        if (CheckCollisionPointRec(mouse, hitBox)) {
            hoveredIndex = i;
        }
        current = current->next;
        i++;
    }
    return hoveredIndex;
}

Rectangle GetDeckRect(GameAssets assets) {
    // Note: On utilise scale = 1.0f comme dans RenderTable
    float scale = 1.0f; 
    float cardW = (assets.cardSheet.width / (float)SHEET_COLS) * scale;
    float cardH = (assets.cardSheet.height / (float)SHEET_ROWS) * scale;
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;
    
    // Position exacte utilisée dans RenderTable pour la pioche
    return (Rectangle){ centerX - cardW - 20, centerY - (cardH/2), cardW, cardH };
}

// Dessine le menu principal
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