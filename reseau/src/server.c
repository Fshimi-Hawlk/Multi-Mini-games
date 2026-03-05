/**
 * @file server.c
 * @brief Serveur RUDP avec Service Discovery et gestion de Lobby.
 * @author i-Charlys (CAILLON Charles)
 * @date 2026-03-05
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdbool.h>

#include "rudp_core.h"
#include "reseauAPI.h"

#define PORT 8080
#define TIMEOUT_US 5000000 // 5 secondes
#define SERVER_NAME "CachyOS-MiniGame-Server"

typedef struct {
    bool active;
    struct sockaddr_in address;
    RUDP_Connection rudp_state;
    struct timeval last_seen;
} UDP_Client;

// --- GLOBALES ---
int master_socket = -1;
UDP_Client clients[MAX_CLIENTS];

/**
 * @brief Recherche ou alloue un slot pour un client basé sur son IP/Port.
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
            RUDP_InitConnection(&clients[i].rudp_state);
            gettimeofday(&clients[i].last_seen, NULL);
            printf("[CONNEXION] Slot %d alloué pour %s:%d\n", i, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
            return i;
        }
    }
    return -1;
}

/**
 * @brief Diffuse un message RUDP à tous les clients sauf l'expéditeur.
 */
void server_broadcast(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len) {
    (void)room_id;
    uint8_t buffer[2048];
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && i != exclude_id) {
            RUDP_Header header;
            RUDP_GenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons((uint16_t)exclude_id); // Identifie qui a bougé
            
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
 * @brief Répond aux requêtes de découverte sans créer de session persistante.
 */
void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDP_Header response;
    // On utilise une connexion temporaire juste pour l'en-tête
    RUDP_Connection temp_conn;
    RUDP_InitConnection(&temp_conn);
    
    RUDP_GenerateHeader(&temp_conn, LOBBY_ROOM_INFO, &response);
    response.sender_id = htons(999); // ID spécial Serveur
    
    uint8_t buffer[512];
    memcpy(buffer, &response, sizeof(RUDP_Header));
    memcpy(buffer + sizeof(RUDP_Header), SERVER_NAME, strlen(SERVER_NAME) + 1);

    sendto(master_socket, buffer, sizeof(RUDP_Header) + strlen(SERVER_NAME) + 1, 0,
           (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
    
    printf("[DISCOVERY] Réponse envoyée à %s\n", inet_ntoa(client_addr->sin_addr));
}

void check_timeouts(void* game_state) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[TIMEOUT] Client %d déconnecté\n", i);
                clients[i].active = false;
                lobby_module.on_player_leave(game_state, i);
            }
        }
    }
}

int main() {
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

    void* game_state = lobby_module.create_instance();
    printf(">>> SERVEUR RUDP DÉMARRÉ SUR LE PORT %d <<<\n", PORT);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);

        struct timeval tv = {0, 16000}; // ~60 FPS tickrate
        int activity = select(master_socket + 1, &readfds, NULL, NULL, &tv);

        if (activity > 0 && FD_ISSET(master_socket, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            uint8_t buffer[2048];
            ssize_t len = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

            if (len >= (ssize_t)sizeof(RUDP_Header)) {
                RUDP_Header *header = (RUDP_Header*)buffer;

                // CAS 1 : Découverte de salon (Broadcast local)
                if (header->action == LOBBY_ROOM_QUERY) {
                    handle_discovery_query(&client_addr);
                } 
                // CAS 2 : Actions de jeu (RUDP filtré)
                else {
                    int id = find_or_create_client(&client_addr);
                    if (id != -1 && RUDP_ProcessIncoming(&clients[id].rudp_state, header)) {
                        gettimeofday(&clients[id].last_seen, NULL);
                        lobby_module.on_action(game_state, id, header->action, 
                                            buffer + sizeof(RUDP_Header), 
                                            len - sizeof(RUDP_Header), server_broadcast);
                    }
                }
            }
        }
        check_timeouts(game_state);
        lobby_module.on_tick(game_state);
    }
    return 0;
}