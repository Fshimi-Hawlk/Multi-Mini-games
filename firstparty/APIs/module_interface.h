/**
 * @file module_interface.h
 * @brief Contrat d'interface pour les modules de jeu (Client-Side).
 */

#ifndef MODULE_INTERFACE_H
#define MODULE_INTERFACE_H

#include <stdint.h>

/**
 * @struct GameTLVHeader
 * @brief Encapsulation dans le payload RUDP.
 * [RUDP_Header] + [GameTLVHeader] + [Données Brutes]
 */
#pragma pack(push, 1)
typedef struct {
    uint8_t  game_id;   // T : Quel mini-jeu (0 = Lobby, 1 = Snake, etc.)
    uint8_t  action;    // T-sub : Quelle action (Move, Shoot, etc.)
    uint16_t length;    // L : Taille de la Value qui suit
} GameTLVHeader;
#pragma pack(pop)

/**
 * @struct MiniGameModule
 * @brief Structure de callbacks que tes collègues doivent remplir.
 */
typedef struct {
    uint8_t id;
    const char* name;
    void (*init)(void);
    void (*on_data)(int player_id, uint8_t action, void* data, uint16_t len);
    void (*update)(float dt);
    void (*draw)(void);
} MiniGameModule;

#endif