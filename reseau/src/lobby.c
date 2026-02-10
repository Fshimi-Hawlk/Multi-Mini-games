#include <stdlib.h> 
#include <string.h> 
#include "protocol.h"       
#include "game_interface.h" 
#include "logger.h"

// On inclut les types partagés (Player_st, Vector2, etc.)
#include "../../lobby/include/utils/userTypes.h" 


// --- STRUCTURE DE L'ÉTAT (State) ---
// C'est la "mémoire" du jeu côté serveur. Elle persiste entre chaque paquet reçu.
typedef struct {
    // On stocke l'état de chaque joueur (position, vie, skin, etc.)
    // MAX_CLIENTS vient de protocol.h (généralement 4 ou 32)
    Player_st players[MAX_CLIENTS]; 
} LobbyState;

/*
   VISUALISATION DE LA MÉMOIRE (LobbyState)
   +-------------------------------------------------------------+
   |  players[0]     |  players[1]     | ... |  players[MAX-1]   |
   +--------+--------+--------+--------+-----+--------+----------+
   | ID: 0  | HP:100 | ID: 1  | HP: 80 |     | ID: 29 | HP: 0    | <--- Player_st
   | X: 10  | Y: 20  | X: 50  | Y: 50  | ... | X: 0   | Y: 0     |
   +--------+--------+--------+--------+-----+--------+----------+
   Le serveur sait où est tout le monde grâce à ce tableau.
*/


// --- CRÉATION DE L'INSTANCE ---
// Appelée une seule fois quand le module est chargé ou que la partie commence.
void* lobby_create() {
    // 1. ALLOCATION : On demande au système de la RAM pour stocker notre LobbyState
    LobbyState* state = (LobbyState*)malloc(sizeof(LobbyState));
    
    // 2. NETTOYAGE : Si l'allocation réussit, on remplit tout de ZÉROS.
    // C'est crucial pour ne pas avoir de "fantômes" (valeurs aléatoires) dans les positions.
    if (state) {
        memset(state, 0, sizeof(LobbyState));
    }

    // On retourne le pointeur vers cette mémoire (le "handle" de l'instance)
    return state;
}


// --- RÉACTION AUX ACTIONS (Le Cerveau) ---
// Appelée à chaque fois que le serveur reçoit un paquet réseau destiné à ce jeu.
void lobby_on_action(void *state, int player_id, uint8_t action, void *payload, uint16_t len, broadcast_func_t broadcast) {
    // On récupère notre mémoire typée (cast de void* vers LobbyState*)
    LobbyState *s = (LobbyState*)state;

    // --- CAS : UN JOUEUR BOUGE ---
    // On vérifie l'action ET la taille des données (sécurité anti-crash)
    if (action == LOBBY_MOVE && len == sizeof(Player_st)) {
        
        // 1. INTERPRÉTATION
        // On considère les données reçues (payload) comme étant une structure Player_st
        Player_st *p = (Player_st*)payload;
        
        // 2. SAUVEGARDE CÔTÉ SERVEUR
        // On met à jour la case mémoire correspondant à CE joueur (player_id)
        // Maintenant, le serveur sait où il est.
        s->players[player_id] = *p;

        // 3. REDIFFUSION (BROADCAST)
        // Le serveur doit dire aux AUTRES "Eh, le joueur X a bougé !"
        // arg1 (0) = room_id (le salon, 0 par défaut ici)
        // arg2 (player_id) = l'ID à EXCLURE (on ne renvoie pas l'info à celui qui l'a envoyée)
        // arg3/4/5 = L'action et les données à transférer
        if (broadcast) {
            broadcast(0, player_id, LOBBY_MOVE, payload, len);
        }
    }
    
    // Tu pourras ajouter d'autres "if" ou "switch" ici pour d'autres actions (TIRER, SAUTER, CHATTER...)
}


// --- BOUCLE DE JEU (Tick) ---
// Appelée régulièrement (ex: 60 fois par seconde) par le serveur principal.
// Sert à gérer la physique, le temps qui passe, les zones de dégâts, etc.
void lobby_tick(void *state) {
    // Pour l'instant vide, car le lobby est juste un "chat visuel" passif.
    // Si on voulait faire bouger des PNJ, c'est ici qu'on le coderait.
    (void)state; // On évite le warning "variable inutilisée"
}


// --- NETTOYAGE (Destruction) ---
// Appelée quand le module est déchargé ou que le serveur s'éteint.
void lobby_destroy(void *state) { 
    if (state) {
        free(state); // On rend la mémoire au système pour éviter les fuites (memory leaks).
    }
}


// --- L'INTERFACE PUBLIQUE (API) ---
// C'est le "Menu" que ton fichier .so / .dll présente au moteur principal.
// Le moteur ne connait pas tes fonctions, il ne connait que cette structure.
GameInterface lobby_module = {
    .game_name = "lobby",           // Nom identifiant le module
    .create_instance = lobby_create,// Pointeur vers la fonction de création
    .on_action = lobby_on_action,   // Pointeur vers la fonction de gestion des paquets
    .on_tick = lobby_tick,          // Pointeur vers la boucle de jeu
    .destroy_instance = lobby_destroy // Pointeur vers le destructeur
    // Note: on_player_leave est manquant ici, assure-toi que ce n'est pas grave
    // dans ton architecture (sinon ajoute .on_player_leave = NULL ou une fonction vide)
};