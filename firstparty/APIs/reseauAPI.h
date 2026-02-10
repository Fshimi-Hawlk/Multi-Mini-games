#ifndef RESEAU_API_H
#define RESEAU_API_H


#include "game_interface.h"  //API

extern GameInterface lobby_module; //format imposé par l'API

#endif // Fin du #ifndef

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