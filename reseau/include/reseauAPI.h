/**
 * @file reseauAPI.h
 * @brief Interface d'abstraction pour l'API de réseau.
 * 
 * Permet au serveur de manipuler différents modules de jeu via 
 * une structure GameServerInterface_St générique.
 * 
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
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
     |                        |            | GameServerInterface_St          |
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
extern GameServerInterface_St lobby_module; 

#endif