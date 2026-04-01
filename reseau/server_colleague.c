/**
 * @file server.c
 * @brief Serveur RUDP autoritaire avec support multi-modules sécurisé.
 * 
 * Ce serveur gère les connexions multiples, le routage des paquets vers 
 * les différents modules de jeu et la diffusion des messages.
 * 
 * Schéma de communication :
 * [CLIENT 1] <----RUDP----> [ SERVEUR ] <----RUDP----> [CLIENT 2]
 *                               |
 *                               v
 *                    +---------------------+
 *                    |   ROUTAGE MODULES   |
 *                    +---------------------+
 *                    | action = SWITCH?    | ----> Change active_module
 *                    | action = GAME_DATA? | ----> on_action(active_game)
 *                    +---------------------+
 * 
 * @author i-Charlys
 * @date 2026-03-18
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#include "networkInterface.h"

#define PORT 8080                                       /**< Port d'écoute du serveur. */
#define TIMEOUT_US 5000000                              /**< Timeout de déconnexion (5s). */
#define SERVER_NAME "CachyOS-KingForFour-Server"        /**< Nom d'affichage du serveur. */
#define MAX_PAYLOAD_SIZE (2048 - sizeof(RUDPHeader_St)) /**< Taille maximale de la charge utile. */

// Interfaces de routage
extern GameServerInterface_St lobby_module; /**< Instance du module lobby. */
extern GameServerInterface_St king_module;  /**< Instance du module King for Four. */

/**
 * @struct UDP_Client
 * @brief Représentation interne d'un client sur le serveur.
 */
typedef struct {
    bool active;                  /**< Si l'emplacement est occupé par un client. */
    struct sockaddr_in address;   /**< Adresse IP et port du client. */
    RUDPConnection_St rudp_state;   /**< État RUDP pour ce client spécifique. */
    struct timeval last_seen;     /**< Horodatage de la dernière activité reçue. */
} UDP_Client;

int master_socket = -1; /**< Socket principal du serveur. */
UDP_Client clients[MAX_CLIENTS]; /**< Liste des clients connectés. */
GameServerInterface_St *active_module = NULL; /**< Pointeur vers le module de jeu actif. */
void* active_game_state = NULL;      /**< État interne du module actif. */

/**
 * @brief Trouve l'index d'un client par son adresse ou lui alloue un nouveau slot.
 * 
 * @param addr Adresse source du paquet.
 * @return int L'index du client (0 à MAX_CLIENTS-1), ou -1 si le serveur est plein.
 */
int find_or_create_client(struct sockaddr_in *addr) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].active && 
           clients[i].address.sin_addr.s_addr == addr->sin_addr.s_addr &&
           clients[i].address.sin_port == addr->sin_port) {
            return i;
        }
    }
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(!clients[i].active) {
            clients[i].active = true;
            clients[i].address = *addr;
            rudpInitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            printf("[CONNEXION] Slot %d alloué pour %s:%d\n", i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }
    return -1;
}

/**
 * @brief Diffuse ou envoie un message à un client spécifique.
 * 
 * @param room_id Identifiant de la salle (-1 pour UNICAST).
 * @param exclude_id ID du client à exclure (en BROADCAST) ou cible unique (en UNICAST).
 * @param action Type d'action à envoyer.
 * @param payload Pointeur vers les données.
 * @param len Taille des données.
 */
void server_broadcast(int room_id, int exclude_id, u8 action, const void *payload, u16 len) {
    u8 buffer[2048];
    if (len > MAX_PAYLOAD_SIZE) len = MAX_PAYLOAD_SIZE;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        bool should_send = false;
        if (room_id == -1) {
            // UNICAST mode: send ONLY to exclude_id (which is now target_id)
            if (i == exclude_id) should_send = true;
        } else {
            // BROADCAST mode: send to all EXCEPT exclude_id
            if (clients[i].active && i != exclude_id) should_send = true;
        }

        if (should_send && clients[i].active) {
            RUDPHeader_St header;
            rudpGenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons((u16)exclude_id); 
            
            memcpy(buffer, &header, sizeof(RUDPHeader_St));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDPHeader_St), payload, len);
            }
            
            sendto(master_socket, buffer, sizeof(RUDPHeader_St) + len, 0, 
                   (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

/**
 * @brief Répond aux requêtes de découverte de serveur (ACTION_CODE_LOBBY_ROOM_QUERY).
 * 
 * @param client_addr Adresse du client demandeur.
 */
void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDPHeader_St response;
    RUDPConnection_St temp_conn;
    rudpInitConnection(&temp_conn);
    
    rudpGenerateHeader(&temp_conn, ACTION_CODE_LOBBY_ROOM_QUERY, &response);
    response.sender_id = htons(999); 
    
    u8 buffer[512];
    memcpy(buffer, &response, sizeof(RUDPHeader_St));
    
    char info[256];
    const char *module_name = (active_module && active_module->game_name) ? active_module->game_name : "Inconnu";
    snprintf(info, sizeof(info), "%s [%s]", SERVER_NAME, module_name);
    
    memcpy(buffer + sizeof(RUDPHeader_St), info, strlen(info) + 1);

    sendto(master_socket, buffer, sizeof(RUDPHeader_St) + strlen(info) + 1, 0,
           (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
}

/**
 * @brief Vérifie les clients inactifs et les déconnecte si nécessaire.
 */
void check_timeouts(void) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[TIMEOUT] Client %d déconnecté\n", i);
                clients[i].active = false;
                
                // On prévient les autres clients
                server_broadcast(0, i, ACTION_CODE_QUIT_GAME, NULL, 0);

                if (active_module && active_module->on_player_leave) {
                    active_module->on_player_leave(active_game_state, i);
                }
            }
        }
    }
}

/**
 * @brief Point d'entrée principal du serveur.
 * 
 * Initialise le socket, les modules et gère la boucle d'événements principale.
 * 
 * @return int Code de sortie.
 */
int main(void) {
    struct sockaddr_in server_addr;
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].active = false;

    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket fail"); exit(1);
    }


    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(master_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fail"); exit(1);
    }

    // Le serveur est forcé de démarrer sur le Lobby
    active_module = &lobby_module;
    
    if (active_module && active_module->create_instance) {
        active_game_state = active_module->create_instance();
    } else {
        printf("[ERREUR] Le module n'implémente pas l'interface `create_instance`.\n");
        exit(1);
    }

    printf(">>> SERVEUR RUDP DÉMARRÉ SUR LE PORT %d <<<\n", PORT);
    printf(">>> MODULE ACTIF : %s <<<\n", active_module->game_name ? active_module->game_name : "NULL");

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        struct timeval tv = {0, 16666}; 
        int activity = select(master_socket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(master_socket, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            u8 buffer[2048];
            ssize_t len = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

            if (len >= (ssize_t)sizeof(RUDPHeader_St)) {
                RUDPHeader_St header;
                memcpy(&header, buffer, sizeof(RUDPHeader_St));

                if (header.action == ACTION_CODE_LOBBY_ROOM_QUERY) {
                    handle_discovery_query(&client_addr);
                } else {
                    int id = find_or_create_client(&client_addr);
                    if (id != -1 && rudpProcessIncoming(&clients[id].rudp_state, &header)) {
                        gettimeofday(&clients[id].last_seen, NULL);
                        
                        if (header.action == ACTION_CODE_LOBBY_SWITCH_GAME) {
                            u8 target_game_id = buffer[sizeof(RUDPHeader_St)];
                            printf("[MODULE] Demande de switch vers ID: %d\n", target_game_id);
                            
                            // Pour simplifier, ID 1 = King for Four
                            if (target_game_id == 1) {
                                if (active_module != &king_module) {
                                    if (active_module && active_module->destroy_instance) {
                                        active_module->destroy_instance(active_game_state);
                                    }
                                    active_module = &king_module;
                                    active_game_state = active_module->create_instance();
                                    printf("[MODULE] Premier switch vers King For Four effectué.\n");
                                }
                                
                                // On envoie TOUJOURS la confirmation au joueur qui demande (Unicast)
                                // Même si le module est déjà actif pour d'autres.
                                u8 switch_payload = 1;
                                server_broadcast(-1, id, ACTION_CODE_LOBBY_SWITCH_GAME, &switch_payload, 1);
                                printf("[MODULE] Confirmation de switch vers King For Four envoyée au client %d.\n", id);
                            }
                        } else if (header.action == ACTION_CODE_LOBBY_CHAT) {
                            server_broadcast(0, id, ACTION_CODE_LOBBY_CHAT, buffer + sizeof(RUDPHeader_St), len - sizeof(RUDPHeader_St));
                        } else if (active_module && active_module->on_action && active_game_state) {
                            active_module->on_action(
                                active_game_state, 
                                id, 
                                header.action, 
                                buffer + sizeof(RUDPHeader_St), 
                                len - sizeof(RUDPHeader_St), 
                                server_broadcast
                            );
                        }
                    }
                }
            }
        }

        check_timeouts();
        if (active_module && active_module->on_tick) {
            active_module->on_tick(active_game_state);
        }
    }

    if (active_module && active_module->destroy_instance) {
        active_module->destroy_instance(active_game_state);
    }
    
    close(master_socket);
    return 0;
}