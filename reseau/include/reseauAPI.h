/**
 * @file reseauAPI.h
 * @brief Interface d'abstraction pour l'API de jeu.
 * * Permet au serveur de manipuler différents modules de jeu via 
 * une structure GameInterface générique.
 */

#ifndef RESEAU_API_H
#define RESEAU_API_H

#include "game_interface.h"

/*
Fichier .H                                 Fichier .C 
     +------------------------+            +------------------------+
     | reseauAPI.h            |            | lobby.c                |
     |                        |            |                        |
     |                        |            |                        |
     |                        |            |                        |
     |                        |            | GameInterface          |
     | extern lobby_module;   | ---------> | lobby_module = {       |
     |           ^            |   Linker   |    .init = ...,        |
     |           |            |            |    .tick = ...         |
     +-----------+------------+            | };                     |
                 |                         +------------------------+
                 |
                 v
      Fichier MAIN.C 
     +------------------------+
     | main.c                 |
     |                        |
     | #include "reseauAPI.h" |
     |                        |
     |                        |
     |                        |
     +------------------------+
     */



/**
 * @brief Instance externe du module lobby.
 * * Cette variable est définie dans lobby.c et liée lors de la compilation.
 */
extern GameInterface lobby_module; 

#endif