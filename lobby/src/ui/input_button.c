/**
 * @file input_button.c
 * @brief Implémentation complète des structures pour une zone de texte et un bouton interactif.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-02-12
 */

#include "../../include/ui/input_button.h"
#include <string.h>
#include <stdlib.h>

int texte_zone = STATE_NORMAL;
int confirmation_button = STATE_DISABLED;

/**
 * @brief Initialise un élément IaC (Input ou Bouton)
 * Alloue la mémoire nécessaire pour le texte et configure la couleur de base.
 */
IaC_button InitIaCElement(float x, float y, float width, float height, char *text, Color color) {
    IaC_button element;
    
    element.rect = (Rectangle){ x, y, width, height };
    element.text = malloc(strlen(text) + 1);
    
    if (element.text != NULL) {
        strcpy(element.text, text);
    }

    element.baseColor = color;
    element.state = STATE_NORMAL;
    element.isIPValid = false; 
    
    return element;
}

/**
 * @brief Gère la saisie de texte pour le champ IP.
 * Écoute les frappes clavier et maintient la saisie sécurisée sous la limite IP_MAX_LENGTH.
 * @return true si le texte a été modifié durant cette frame
 */
bool UpdateIPInput(IaC_button *input, char *buffer, int *letterCount) {
    bool isModified = false;
    Vector2 mousePoint = GetMousePosition();

    // Gestion du focus via le curseur et le clic de la souris
    if (CheckCollisionPointRec(mousePoint, input->rect)) {
        if (input->state != STATE_ACTIVE) input->state = STATE_HOVER;
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_ACTIVE;
        }
    } else {
        if (input->state == STATE_HOVER) input->state = STATE_NORMAL;
        
        // Perte du focus (blur) si l'utilisateur clique en dehors de la zone
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            input->state = STATE_NORMAL;
        }
    }

    // Gestion de la frappe clavier uniquement lorsque l'input a le focus
    if (input->state == STATE_ACTIVE) {
        int key = GetCharPressed();
        
        // Traitement de multiples frappes pouvant survenir dans la même frame
        while (key > 0) {
            // Filtrage strict : uniquement les chiffres (48-57) et le point (46)
            if ((key >= 48 && key <= 57) || key == 46) {
                if (*letterCount < IP_MAX_LENGTH) {
                    buffer[*letterCount] = (char)key;
                    buffer[*letterCount + 1] = '\0';
                    (*letterCount)++;
                    isModified = true;
                }
            }
            key = GetCharPressed();
        }

        // Action destructive : retour arrière
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            if (*letterCount > 0) {
                (*letterCount)--;
                buffer[*letterCount] = '\0';
                isModified = true;
            }
        }
    }
    
    // Règle métier : une IPv4 typique compte un minimum de 7 caractères (ex: 1.1.1.1)
    input->isIPValid = (*letterCount >= 7); 

    return isModified;
}

/**
 * @brief Gère l'interaction complète avec le bouton de connexion.
 * Incorpore une vérification pour désactiver l'élément si canClick est faux.
 * @return true si le clic est confirmé (bouton relâché sur l'élément)
 */
bool UpdateConnectButton(IaC_button *button, bool canClick) {
    if (!canClick) {
        button->state = STATE_DISABLED;
        return false;
    }

    Vector2 mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, button->rect)) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            button->state = STATE_CLICK;
        } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            button->state = STATE_HOVER;
            return true; // Action utilisateur finale déclenchée
        } else {
            button->state = STATE_HOVER;
        }
    } else {
        button->state = STATE_NORMAL;
    }
    
    return false;
}

/**
 * @brief Moteur de rendu de l'élément à l'écran, gérant dynamiquement les retours visuels
 * selon l'état interactif et affichant le texte ou le placeholder approprié.
 */
void DrawIaCElement(IaC_button element, const char *currentText) {
    Color drawColor = element.baseColor;
    Color textColor = BLACK;
    
    // Application des teintes selon l'automate fini (cycle de vie UI)
    switch (element.state) {
        case STATE_HOVER:    drawColor = Fade(element.baseColor, 0.8f); break;
        case STATE_ACTIVE:   drawColor = Fade(element.baseColor, 1.0f); break;
        case STATE_CLICK:    drawColor = Fade(element.baseColor, 0.6f); break;
        case STATE_DISABLED: drawColor = LIGHTGRAY; textColor = GRAY;   break;
        case STATE_NORMAL:   drawColor = Fade(element.baseColor, 0.9f); break;
    }

    // Peinture de l'arrière-plan
    DrawRectangleRec(element.rect, drawColor);
    
    // Contour dynamique offrant une indication claire d'accessibilité (Focus)
    Color outlineColor = (element.state == STATE_ACTIVE) ? RED : DARKGRAY;
    DrawRectangleLines((int)element.rect.x, (int)element.rect.y, (int)element.rect.width, (int)element.rect.height, outlineColor);

    // Résolution du texte : Tampon actif ou Placeholder initial
    const char* textToDraw = (currentText != NULL && strlen(currentText) > 0) ? currentText : element.text;
    int textWidth = MeasureText(textToDraw, 20);
    
    // Par défaut, le bouton requiert un centrage parfait
    int textX = (int)(element.rect.x + (element.rect.width - textWidth) / 2);
    int textY = (int)(element.rect.y + (element.rect.height - 20) / 2);
    
    // Pour une zone de saisie, on force l'alignement sur le bord gauche
    if (currentText != NULL) {
        textX = (int)(element.rect.x + 10);
        // Diminution de l'opacité si le tampon est vide et qu'il s'agit du placeholder
        if (strlen(currentText) == 0) textColor = GRAY;
    }
    
    DrawText(textToDraw, textX, textY, 20, textColor);
    
    // Rendu d'un curseur de frappe oscillant dans la zone active
    if (element.state == STATE_ACTIVE && currentText != NULL) {
        if (((int)(GetTime() * 2)) % 2 == 0) { // Oscillation temporelle stricte basée sur l'horloge interne
            DrawRectangle(textX + textWidth + 2, textY, 2, 20, RED);
        }
    }
}