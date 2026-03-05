/**
 * @file connection_screen.h
 * @brief Déclaration de la scène de connexion pour la saisie IP.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#ifndef CONNECTION_SCREEN_H
#define CONNECTION_SCREEN_H

#include <stdbool.h>



const char* GetEnteredIP(void);

/**
 * @brief Alloue et positionne les éléments UI pour l'écran de connexion.
 */
void InitConnectionScreen(void);

/**
 * @brief Boucle logique de l'écran de connexion.
 * @return true si la connexion est déclenchée (IP valide + Clic).
 */
bool UpdateConnectionScreen(void);

/**
 * @brief Moteur de rendu de l'écran de connexion.
 */
void DrawConnectionScreen(void);

#endif // CONNECTION_SCREEN_H