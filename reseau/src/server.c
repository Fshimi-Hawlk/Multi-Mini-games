/**
 * @file server.c
 * @brief Serveur RUDP autoritaire avec support multi-modules sécurisé.
 * 
 * Ce serveur gère les connexions multiples, le routage des paquets vers 
 * les différents modules de jeu et la diffusion des messages.
 * 
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-18
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include "rudp_core.h"
#include "game_interface.h"
#include "reseauAPI.h"
#include "APIs/generalAPI.h"

#define PORT 8080
#define MAX_PAYLOAD_SIZE 1024
#define SERVER_NAME "MMG-Official-Server"
#define TIMEOUT_US 120000000 /**< Timeout de déconnexion en microsecondes (120s). */

/**
 * @struct UDP_Client
 * @brief Représentation d'un client connecté côté serveur.
 */
typedef struct {
    struct sockaddr_in address;   /**< Adresse réseau du client. */
    RUDP_Connection rudp_state;   /**< État du protocole RUDP pour ce client. */
    struct timeval last_seen;     /**< Horodatage du dernier paquet reçu. */
    u8 current_game_id;           /**< ID du jeu/salon actuel (0: Lobby). */
    bool active;                  /**< Indicateur d'activité de l'emplacement. */
} UDP_Client;

// --- Variables Globales ---
s32 master_socket;
UDP_Client clients[MAX_CLIENTS];
GameInterface* active_module = NULL;
void* active_game_state = NULL;

extern GameInterface king_module;
extern GameInterface lobby_module;

/**
 * @brief Trouve l'index d'un client par son adresse, ou en crée un nouveau.
 */
s32 find_or_create_client(struct sockaddr_in *addr) {
    for(s32 i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && 
            clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
            clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }
    for(s32 i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
            clients[i].address = *addr;
            RUDP_InitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            clients[i].current_game_id = 0; // Par défaut dans le lobby
            clients[i].active = true;
            return i;
        }
    }
    return -1;
}

/**
 * @brief Diffuse un message à un ou plusieurs clients avec isolation de room.
 * 
 * @param target_id ID du client cible (Unicast), ou -1 pour broadcast room, ou -2 pour global.
 * @param sender_id ID de l'émetteur original (999 pour le serveur).
 * @param action Code d'action RUDP.
 * @param payload Données à envoyer.
 * @param len Taille des données.
 */
void server_broadcast(s32 target_id, s32 sender_id, u8 action, void *payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    // Déterminer la room de l'émetteur pour le filtrage
    u8 room_id = 0;
    if (sender_id == 999) {
        // Le serveur parle : on utilise la room du module actif
        room_id = (active_module == &king_module) ? 1 : 0;
    } else if (sender_id >= 0 && sender_id < MAX_CLIENTS && clients[sender_id].active) {
        room_id = clients[sender_id].current_game_id;
    }

    for (s32 i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) continue;

        bool should_send = false;
        if (target_id == -1) {
            // Broadcast Room : uniquement aux joueurs dans la MÊME room SAUF l'émetteur
            if (clients[i].current_game_id == room_id && i != sender_id) should_send = true;
        } else if (target_id == -2) {
            // Broadcast Global : à TOUT LE MONDE
            should_send = true;
        } else {
            // Unicast : uniquement à la cible
            if (i == target_id) should_send = true;
        }

        if (should_send) {
            RUDP_Header header;
            RUDP_GenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons((u16)sender_id); 
            
            memcpy(buffer, &header, sizeof(RUDP_Header));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDP_Header), payload, len);
            }
            
            sendto(master_socket, buffer, sizeof(RUDP_Header) + len, 0, 
                   (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

/**
 * @brief Répond aux requêtes de découverte de serveur (LOBBY_ROOM_QUERY).
 */
void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDP_Header response;
    RUDP_Connection temp_conn;
    RUDP_InitConnection(&temp_conn);
    RUDP_GenerateHeader(&temp_conn, LOBBY_ROOM_INFO, &response);
    response.sender_id = htons(999); 
    
    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDP_Header));
    
    char info[256];
    const char *module_name = (active_module && active_module->game_name) ? active_module->game_name : "Inconnu";
    snprintf(info, sizeof(info), "%s [%s]", SERVER_NAME, module_name);
    memcpy(buffer + sizeof(RUDP_Header), info, strlen(info) + 1);
    
    sendto(master_socket, buffer, sizeof(RUDP_Header) + strlen(info) + 1, 0, 
           (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief Vérifie les clients inactifs et les déconnecte après un délai.
 */
void check_timeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (s32 i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[TIMEOUT] Client %d déconnecté.\n", i);
                clients[i].active = false;
                server_broadcast(-1, i, LOBBY_LEAVE, NULL, 0);
                if (active_module && active_module->on_player_leave) {
                    active_module->on_player_leave(active_game_state, i);
                }
            }
        }
    }
}

s32 main(void) {
    struct sockaddr_in server_addr;
    for (s32 i = 0; i < MAX_CLIENTS; i++) clients[i].active = false;

    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket fail"); exit(1);
    }

    s32 opt = 1;
    setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fail"); exit(1);
    }

    active_module = &lobby_module;
    active_game_state = active_module->create_instance();

    printf(">>> SERVEUR RUDP DÉMARRÉ SUR LE PORT %d <<<\n", PORT);

    struct timeval last_tick;
    gettimeofday(&last_tick, NULL);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        struct timeval tv = {0, 1000}; // Attente courte (1ms)
        s32 activity = select(master_socket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(master_socket, &readfds)) {
            while (1) {
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                u8 buffer[2048];
                // Utilise MSG_DONTWAIT pour vider le buffer sans bloquer
                ssize_t len = recvfrom(master_socket, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr*)&client_addr, &addr_len);
                
                if (len < 0) break; // Plus de données à lire
                if (len < (ssize_t)sizeof(RUDP_Header)) continue;

                RUDP_Header header;
                memcpy(&header, buffer, sizeof(RUDP_Header));

                if (header.action == LOBBY_ROOM_QUERY) {
                    handle_discovery_query(&client_addr);
                } 
                else {
                    s32 id = find_or_create_client(&client_addr);
                    if (id != -1 && RUDP_ProcessIncoming(&clients[id].rudp_state, &header)) {
                        gettimeofday(&clients[id].last_seen, NULL);
                        
                        s32 sid = ntohs(header.sender_id);

                        if (header.action == LOBBY_JOIN) {
                            printf("[LOBBY] Client %d a rejoint.\n", id);
                            clients[id].current_game_id = 0;
                            u16 assigned_id = (u16)id;
                            server_broadcast(id, id, LOBBY_JOIN, &assigned_id, sizeof(u16));
                            
                            // Notifier le module actif pour qu'il synchronise les données initiales
                            if (active_module && active_module->on_action && active_game_state) {
                                active_module->on_action(active_game_state, id, header.action, NULL, 0, server_broadcast);
                            }
                        }
                        else if (header.action == LOBBY_SWITCH_GAME) {
                            u8 target_game_id = buffer[sizeof(RUDP_Header)];
                            printf("[SYSTEM] Client %d demande switch vers jeu %d\n", id, target_game_id);
                            
                            clients[id].current_game_id = target_game_id;

                            if (target_game_id == 1) {
                                if (active_module != &king_module) {
                                    if (active_module && active_module->destroy_instance) {
                                        active_module->destroy_instance(active_game_state);
                                    }
                                    active_module = &king_module;
                                    active_game_state = active_module->create_instance();
                                }
                                u8 switch_payload = 1;
                                server_broadcast(id, 999, LOBBY_SWITCH_GAME, &switch_payload, 1);
                            }
                        }
                        else if (header.action == LOBBY_MOVE || header.action == LOBBY_LEAVE) {
                            // Relais immédiat aux autres dans la room
                            server_broadcast(-1, id, header.action, buffer + sizeof(RUDP_Header), (u16)(len - sizeof(RUDP_Header)));
                        }
                        else if (header.action == 5 /* LOBBY_CHAT / ACTION_GAME_DATA */) {
                            if (len >= (ssize_t)(sizeof(RUDP_Header) + sizeof(GameTLVHeader))) {
                                GameTLVHeader* tlv = (GameTLVHeader*)(buffer + sizeof(RUDP_Header));
                                
                                if (tlv->action == LOBBY_CHAT) {
                                    server_broadcast(-1, id, 5, buffer + sizeof(RUDP_Header), (u16)(len - sizeof(RUDP_Header)));
                                } 
                                else if (active_module && active_module->on_action && active_game_state) {
                                    active_module->on_action(active_game_state, id, header.action, buffer + sizeof(RUDP_Header), (u16)(len - sizeof(RUDP_Header)), server_broadcast);
                                }
                            }
                        }
                        else if (active_module && active_module->on_action && active_game_state) {
                            active_module->on_action(active_game_state, id, header.action, buffer + sizeof(RUDP_Header), (u16)(len - sizeof(RUDP_Header)), server_broadcast);
                        }
                    }
                }
            }
        }

        struct timeval now;
        gettimeofday(&now, NULL);
        long long elapsed = (now.tv_sec - last_tick.tv_sec) * 1000000LL + (now.tv_usec - last_tick.tv_usec);
        
        // Tick à 60Hz (16666 us)
        if (elapsed >= 16666) {
            check_timeouts();
            if (active_module && active_module->on_tick) {
                active_module->on_tick(active_game_state, server_broadcast);
            }
            last_tick = now;
        }
    }

    close(master_socket);
    return 0;
}
