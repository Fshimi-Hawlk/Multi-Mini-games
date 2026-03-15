/**
 * @file server.c
 * @brief Serveur RUDP autoritaire avec support multi-modules sécurisé.
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
#include <sys/select.h>

#include "rudp_core.h"
#include "reseauAPI.h"
#include "game_interface.h"

#define PORT 8080
#define TIMEOUT_US 5000000000
#define SERVER_NAME "CachyOS-KingForFour-Server"
#define MAX_PAYLOAD_SIZE (2048 - sizeof(RUDP_Header))

// Interfaces de routage
extern GameInterface lobby_module; 
extern GameInterface king_module;

typedef struct {
    bool active;
    struct sockaddr_in address;
    RUDP_Connection rudp_state;
    struct timeval last_seen;
} UDP_Client;

int master_socket = -1;
UDP_Client clients[MAX_CLIENTS];
GameInterface *active_module = NULL; 
void* active_game_state = NULL;      

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

void server_broadcast(int room_id, int exclude_id, uint8_t action, void *payload, uint16_t len) {
    uint8_t buffer[2048];
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
            RUDP_Header header;
            RUDP_GenerateHeader(&clients[i].rudp_state, action, &header);
            header.sender_id = htons((uint16_t)exclude_id); 
            
            memcpy(buffer, &header, sizeof(RUDP_Header));
            if (len > 0 && payload != NULL) {
                memcpy(buffer + sizeof(RUDP_Header), payload, len);
            }
            
            sendto(master_socket, buffer, sizeof(RUDP_Header) + len, 0, 
                   (struct sockaddr *)&clients[i].address, sizeof(struct sockaddr_in));
        }
    }
}

void handle_discovery_query(struct sockaddr_in *client_addr) {
    RUDP_Header response;
    RUDP_Connection temp_conn;
    RUDP_InitConnection(&temp_conn);
    
    RUDP_GenerateHeader(&temp_conn, LOBBY_ROOM_INFO, &response);
    response.sender_id = htons(999); 
    
    uint8_t buffer[512];
    memcpy(buffer, &response, sizeof(RUDP_Header));
    
    char info[256];
    const char *module_name = (active_module && active_module->game_name) ? active_module->game_name : "Inconnu";
    snprintf(info, sizeof(info), "%s [%s]", SERVER_NAME, module_name);
    
    memcpy(buffer + sizeof(RUDP_Header), info, strlen(info) + 1);

    sendto(master_socket, buffer, sizeof(RUDP_Header) + strlen(info) + 1, 0,
           (struct sockaddr *)client_addr, sizeof(struct sockaddr_in));
}

void check_timeouts() {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) {
            long long elapsed = (now.tv_sec - clients[i].last_seen.tv_sec) * 1000000LL +
                                (now.tv_usec - clients[i].last_seen.tv_usec);
            if (elapsed > TIMEOUT_US) {
                printf("[TIMEOUT] Client %d déconnecté\n", i);
                clients[i].active = false;
                if (active_module && active_module->on_player_leave) {
                    active_module->on_player_leave(active_game_state, i);
                }
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
            uint8_t buffer[2048];
            ssize_t len = recvfrom(master_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

            if (len >= (ssize_t)sizeof(RUDP_Header)) {
                RUDP_Header header;
                memcpy(&header, buffer, sizeof(RUDP_Header));

                if (header.action == LOBBY_ROOM_QUERY) {
                    handle_discovery_query(&client_addr);
                } 
                else {
                    int id = find_or_create_client(&client_addr);
                    if (id != -1 && RUDP_ProcessIncoming(&clients[id].rudp_state, &header)) {
                        gettimeofday(&clients[id].last_seen, NULL);
                        
                        if (header.action == LOBBY_SWITCH_GAME) {
                            uint8_t target_game_id = buffer[sizeof(RUDP_Header)];
                            printf("[MODULE] Demande de switch vers ID: %d\n", target_game_id);
                            
                            // Pour simplifier, ID 1 = King for Four
                            if (target_game_id == 1 && active_module != &king_module) {
                                if (active_module && active_module->destroy_instance) {
                                    active_module->destroy_instance(active_game_state);
                                }
                                active_module = &king_module;
                                active_game_state = active_module->create_instance();
                                printf("[MODULE] Changement vers King For Four effectué.\n");
                                
                                // On broadcast le switch à TOUS
                                server_broadcast(0, -1, LOBBY_SWITCH_GAME, &target_game_id, 1);
                            }
                        }
                        else if (active_module && active_module->on_action) {
                            active_module->on_action(
                                active_game_state, 
                                id, 
                                header.action, 
                                buffer + sizeof(RUDP_Header), 
                                len - sizeof(RUDP_Header), 
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