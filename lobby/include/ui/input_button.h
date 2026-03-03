/**
 * @file input_button.h
 * @brief Définition de la structure IaC (Input and Connect)
 * @author i-Charlys (CAILLO)
 * @date 2026-02-12
 */

#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include "../../../thirdparty/raylib.h" 
#include <stdbool.h>

#define IP_MAX_LENGTH 15

// États pour la zone de texte et le bouton
typedef enum {
    STATE_NORMAL = 0,   // Par défaut
    STATE_HOVER,        // Souris dessus
    STATE_ACTIVE,       // En train d'écrire (pour l'input)
    STATE_CLICK,        // Cliqué (pour le bouton)
    STATE_DISABLED      // Désactivé (ex: bouton si IP invalide)
} ButtonState;

// Structure Input and Connect
typedef struct {
    Rectangle rect;
    ButtonState state;
    Color baseColor;
    char *text;   // Label ou placeholder
    bool isIPValid;     // État de validation de l'IP saisie
} IaC_button;

/**
 * @brief Initialise un élément IaC (Input ou Bouton)
 */
IaC_button InitIaCElement(float x, float y, float width, float height, char *text, Color color);

/**
 * @brief Gère la saisie de texte pour le champ IP
 * @return true si le texte a été modifié
 */
bool UpdateIPInput(IaC_button *input, char *buffer, int *letterCount);

/**
 * @brief Gère l'interaction avec le bouton de connexion
 * @return true si le bouton est cliqué ET que l'IP est valide
 */
bool UpdateConnectButton(IaC_button *button, bool canClick);

/**
 * @brief Affiche l'élément à l'écran
 */
void DrawIaCElement(IaC_button element, const char *currentText);

#endif