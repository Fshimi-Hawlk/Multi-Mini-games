/**
 * @file reseauAPI.h
 * @brief Permet de faire le lien entre lobby.c et main.c.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-02-07
 */

#ifndef RESEAU_API_H
#define RESEAU_API_H

#include "game_interface.h"

/**
 * @brief Instance globale du module lobby, exportée pour le linker.
 */
extern GameInterface lobby_module; 

#endif

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