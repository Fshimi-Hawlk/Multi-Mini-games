// common/game_interface.h
#ifndef GAME_INTERFACE_H
#define GAME_INTERFACE_H

#include <stdint.h>

// Tous tes jeux (Lobby inclus) doivent respecter cette structure
typedef struct GameInterface {
    char *game_name; // "lobby", "snake", "uno"

    // 1. Init : Crée les données d'une nouvelle partie (malloc)
    void* (*create_instance)();

    // 2. Action : Reçoit un paquet d'un joueur
    void  (*on_action)(void *state, int player_id, uint8_t action, void *payload, uint16_t len);

    // 3. Tick : Appelé en boucle (pour la physique, collisions...)
    void  (*on_tick)(void *state);

    // 4. Départ : Un joueur quitte ou déco
    void  (*on_player_leave)(void *state, int player_id);
    
    // 5. Nettoyage : Fin de partie (free)
    void  (*destroy_instance)(void *state);

} GameInterface;

#endif