/**
    @file reseauAPI.h
    @author Charles CAILLON
    @date 2026-03-18
    @date 2026-04-14
    @brief Interface d'abstraction pour l'API de réseau.
*/
#ifndef RESEAU_API_H
#define RESEAU_API_H

#include "networkInterface.h"

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