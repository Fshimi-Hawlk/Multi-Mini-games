/**
 * @file connection_screen.h
 * @brief Déclaration de la scène de connexion pour la saisie IP.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#ifndef CONNECTION_SCREEN_H
#define CONNECTION_SCREEN_H

#define LOBBY_ROOM_QUERY 3  // Client -> Réseau (Où sont les serveurs ?)
#define LOBBY_ROOM_INFO  4  // Serveur -> Client (Je suis ici : IP + Nom)
#define ACTION_GAME_DATA  5  

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

/**
 * @brief Ajoute un serveur découvert à la liste UI.
 * Cette déclaration permet à main.c de l'appeler.
 */
void AddDiscoveredRoom(const char* ip, const char* name);

#endif // CONNECTION_SCREEN_H