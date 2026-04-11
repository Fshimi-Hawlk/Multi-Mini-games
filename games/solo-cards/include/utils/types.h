/**
 * @file types.h
 * @brief Type definitions for Solitaire game
 * 
 * Ce fichier contient les définitions de types de base
 * utilisés dans le jeu de Solitaire.
 * 
 * @author Multi Mini-Games Team
 * @date February 2026
 */

#ifndef UTILS_TYPES_H
#define UTILS_TYPES_H

#include "common.h"
#include "configs.h"

/**
 * @brief Vecteur 2D avec des coordonnées entières
 * 
 * Utilisé pour les positions et les décalages dans la grille.
 */
typedef struct iVector2 {
    int x;  // Coordonnée X
    int y;  // Coordonnée Y
} iVector2;

#endif // UTILS_TYPES_H
