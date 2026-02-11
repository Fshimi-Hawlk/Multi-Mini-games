/**
 * @file main.c
 * @brief Point d'entrée du client Lobby avec gestion réseau (Sockets TCP).
 * @author Fshimi Hawlk
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-01-7
 * * Ce fichier gère l'affichage Raylib, la boucle de jeu locale et la
 * synchronisation des positions des joueurs via le serveur.
 */

#include "core/game.h"
#include "ui/app.h"
// ... (autres includes)

/**
 * @brief Initialise la connexion TCP vers le serveur.
 * * Configure la socket, tente la connexion, passe la socket en mode
 * non-bloquant et envoie le paquet initial LOBBY_JOIN.
 */
void init_network() {
    // ... implémentation
}

/**
 * @brief Envoie la position locale au serveur.
 * @param p Pointeur vers la structure du joueur local.
 * * Encapsule les données du joueur dans un PacketHeader avec l'action LOBBY_MOVE.
 */
void send_my_position(Player_st *p) {
    // ... implémentation
}

/**
 * @brief Vérifie et traite les données reçues sur la socket.
 * * Lit l'en-tête, puis le payload. Si une mise à jour de position est reçue,
 * elle est stockée dans le tableau otherPlayers avec une texture locale.
 */
void receive_network_data() {
    // ... implémentation
}

/**
 * @brief Boucle de mise à jour et de rendu (Tick client).
 * @param dt Delta time (temps écoulé depuis la dernière frame).
 * * Gère la physique locale, l'envoi réseau, la réception et le dessin de la scène.
 */
static void lobby_gameLoop(float dt) {
    // ... implémentation
}

/**
 * @brief Entrée principale : initialise la fenêtre et la boucle Raylib.
 */
int main(void) {
    // ... implémentation
}