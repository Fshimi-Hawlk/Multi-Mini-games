/**
 * @file module_interface.h
 * @brief Interface standardisée pour les mini-jeux côté Client (Frontend Raylib).
 * @note Fichier reconstruit suite à la purge du Makefile.
 */

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <stdint.h>
#include <stddef.h>

// --- STRUCTURE RÉSEAU INTERNE (Payload) ---
// Désactivation du padding mémoire pour correspondre exactement aux octets réseau (1+1+2 = 4 octets)
#pragma pack(push, 1)
typedef struct {
    uint8_t game_id;     // ID du mini-jeu ciblé (0 pour le Lobby)
    uint8_t action;      // Action spécifique au jeu (ex: 2 pour LOBBY_MOVE)
    uint16_t length;     // Taille des données qui suivent cet en-tête
} GameTLVHeader;
#pragma pack(pop)

// --- INTERFACE DU MODULE GRAPHIQUE ---
/**
 * @struct MiniGameModule
 * @brief Contrat d'interface que chaque mini-jeu client doit respecter.
 */
typedef struct MiniGameModule {
    uint8_t id;          // Identifiant réseau du jeu
    const char* name;    // Nom d'affichage
    
    /** @brief Allocation et chargement des textures/modèles (Raylib). */
    void (*init)(void);
    
    /** @brief Réception d'un paquet réseau routé pour ce jeu. */
    void (*on_data)(int player_id, uint8_t action, void* data, uint16_t len);
    
    /** @brief Mise à jour de la logique physique locale (Frame par Frame). */
    void (*update)(float dt);
    
    /** @brief Rendu graphique à l'écran (Raylib). */
    void (*draw)(void);
} MiniGameModule;

#endif // MODULE_INTERFACE_H