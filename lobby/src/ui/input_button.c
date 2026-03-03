/**
 * @file main.c
 * @brief definition des structures pour une zone de texte.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-02-12
 * Ce fichier gère la structure du bouton de connexion
 */
 
 #include "../../include/ui/input_button.h"
 #include <string.h>
 #include <stdlib.h>
 
 
 int texte_zone = STATE_NORMAL;
 int confirmation_button = STATE_DISABLED;
 
/**
 * @brief Initialise un élément IaC (Input ou Bouton)
 */
 IaC_button InitIaCElement(float x, float y, float width, float height, char *text, Color color) {
     IaC_button element;
     
     // Initialisation du rectangle
     element.rect = (Rectangle){ x, y, width, height };
     
     // Allocation dynamique de la mémoire pour le texte
     element.text = malloc(strlen(text) + 1);
     
     if (element.text != NULL) {
         strcpy(element.text, text);
     }
 
     element.baseColor = color;
     element.state = STATE_NORMAL;
     element.isIPValid = false; // Initialisation par défaut
     
     return element;
 }
 
 /**
  * @brief Gère l'interaction avec le bouton de connexion
  * @return true si le bouton est cliqué ET que l'IP est valide
  */
 bool UpdateConnectButton(IaC_button *button, bool canClick){
     if (canClick && button->state == STATE_NORMAL) {
         button->state = STATE_CLICKED;
         return true;
     }
     return false;
 }
 
 /**
  * @brief Affiche l'élément à l'écran
  */
 void DrawIaCElement(IaC_button element, const char *currentText);
