/**
    @file types.h
    @author Maxime CHAUVEAU
    @date February 2026
    @date 2026-04-07
    @brief Type definitions for Solitaire game
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
